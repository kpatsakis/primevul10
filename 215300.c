iasecc_compute_signature_at(struct sc_card *card,
		const unsigned char *in, size_t in_len, unsigned char *out, size_t out_len)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_private_data *prv = (struct iasecc_private_data *) card->drv_data;
	struct sc_security_env *env = &prv->security_env;
	struct sc_apdu apdu;
	size_t offs = 0, sz = 0;
	unsigned char rbuf[SC_MAX_APDU_BUFFER_SIZE];
	int rv;

	LOG_FUNC_CALLED(ctx);
	if (env->operation != SC_SEC_OPERATION_AUTHENTICATE)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "It's not SC_SEC_OPERATION_AUTHENTICATE");

	sc_format_apdu(card, &apdu, SC_APDU_CASE_4_SHORT, 0x88, 0x00, 0x00);
	apdu.datalen = in_len;
	apdu.data = in;
	apdu.lc = in_len;
	apdu.resp = rbuf;
	apdu.resplen = sizeof(rbuf);
	apdu.le = 0x100;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "Compute signature failed");

	do   {
		if (offs + apdu.resplen > out_len)
			LOG_TEST_RET(ctx, SC_ERROR_BUFFER_TOO_SMALL, "Buffer too small to return signature");

		memcpy(out + offs, rbuf, apdu.resplen);
		offs += apdu.resplen;

		if (apdu.sw1 == 0x90 && apdu.sw2 == 0x00)
			break;

		if (apdu.sw1 == 0x61)   {
			sz = apdu.sw2 == 0x00 ? 0x100 : apdu.sw2;
			rv = iso_ops->get_response(card, &sz, rbuf);
			LOG_TEST_RET(ctx, rv, "Get response error");

			apdu.resplen = rv;
		}
		else   {
			LOG_TEST_RET(ctx, SC_ERROR_INTERNAL, "Impossible error: SW1 is not 0x90 neither 0x61");
		}

	} while(rv > 0);

	LOG_FUNC_RETURN(ctx, offs);
}