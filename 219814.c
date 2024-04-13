static struct htx_sl *h2_prepare_htx_stsline(uint32_t fields, struct ist *phdr, struct htx *htx, unsigned int *msgf)
{
	unsigned int status, flags = HTX_SL_F_NONE;
	struct htx_sl *sl;
	struct ist stat;

	/* only :status is allowed as a pseudo header */
	if (!(fields & H2_PHDR_FND_STAT))
		goto fail;

	if (phdr[H2_PHDR_IDX_STAT].len != 3)
		goto fail;

	/* if Extended CONNECT is used, convert status code from 200 to htx 101
	 * following rfc 8441 */
	if (unlikely(*msgf & H2_MSGF_EXT_CONNECT) &&
	    isteq(phdr[H2_PHDR_IDX_STAT], ist("200"))) {
		stat = ist("101");
		status = 101;
	}
	else {
		unsigned char h, t, u;

		stat = phdr[H2_PHDR_IDX_STAT];

		h = stat.ptr[0] - '0';
		t = stat.ptr[1] - '0';
		u = stat.ptr[2] - '0';
		if (h > 9 || t > 9 || u > 9)
			goto fail;
		status = h * 100 + t * 10 + u;
	}

	/* 101 responses are not supported in H2, so return a error.
	 * On 1xx responses there is no ES on the HEADERS frame but there is no
	 * body. So remove the flag H2_MSGF_BODY and add H2_MSGF_RSP_1XX to
	 * notify the decoder another HEADERS frame is expected.
	 * 204/304 response have no body by definition. So remove the flag
	 * H2_MSGF_BODY and set H2_MSGF_BODYLESS_RSP.
	 *
	 * Note however that there is a special condition for Extended CONNECT.
	 * In this case, we explicitly convert it to HTX 101 to mimic
	 * Get+Upgrade HTTP/1.1 mechanism
	 */
	if (status == 101) {
		if (!(*msgf & H2_MSGF_EXT_CONNECT))
			goto fail;
	}
	else if (status < 200) {
		*msgf |= H2_MSGF_RSP_1XX;
		*msgf &= ~H2_MSGF_BODY;
	}
	else if (status == 204 || status == 304) {
		*msgf &= ~H2_MSGF_BODY;
		*msgf |= H2_MSGF_BODYLESS_RSP;
	}

	/* Set HTX start-line flags */
	flags |= HTX_SL_F_VER_11;    // V2 in fact
	flags |= HTX_SL_F_XFER_LEN;  // xfer len always known with H2

	sl = htx_add_stline(htx, HTX_BLK_RES_SL, flags, ist("HTTP/2.0"), stat, ist(""));
	if (!sl)
		goto fail;
	sl->info.res.status = status;
	return sl;
 fail:
	return NULL;
}