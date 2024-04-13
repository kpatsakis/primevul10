static int cardos_construct_fcp(sc_card_t *card, const sc_file_t *file,
	u8 *out, size_t *outlen)
{
	u8     buf[64], *p = out;
	size_t inlen = *outlen, len;
	int    r;

	LOG_FUNC_CALLED(card->ctx);

	if (out == NULL || inlen < 64)
		return SC_ERROR_INVALID_ARGUMENTS;
	/* add FCP tag */
	*p++ = 0x62;
	/* we will add the length later  */
	p++;

	memset(buf, 0, sizeof(buf));

	/* set the length */
	buf[0] = (file->size >> 8) & 0xff;
	buf[1] = file->size        & 0xff;
	if (file->type == SC_FILE_TYPE_DF)
		r = sc_asn1_put_tag(0x81, buf, 2, p, 4, &p);
	else
		r = sc_asn1_put_tag(0x80, buf, 2, p, 4, &p);
	if (r != SC_SUCCESS)
		return r;
	/* set file type  */
	if (file->shareable != 0)
		buf[0] = 0x40;
	else
		buf[0] = 0x00;
	if (file->type == SC_FILE_TYPE_WORKING_EF) {
		switch (file->ef_structure) {
		case SC_FILE_EF_TRANSPARENT:
			buf[0] |= 0x01;
			break;
		case SC_FILE_EF_LINEAR_VARIABLE_TLV:
			buf[0] |= 0x05;
			break;
		case SC_FILE_EF_LINEAR_FIXED:
			buf[0] |= 0x02;
			buf[1] |= 0x21;
			buf[2] |= 0x00;
			buf[3] |= (u8) file->record_length;
			buf[4] |= (u8) file->record_count;
			break;
		case SC_FILE_EF_CYCLIC:
			buf[0] |= 0x06;
			buf[1] |= 0x21;
			buf[2] |= 0x00;
			buf[3] |= (u8) file->record_length;
			buf[4] |= (u8) file->record_count;
			break;
		default:
			sc_log(card->ctx,  "unknown EF type: %u", file->type);
			return SC_ERROR_INVALID_ARGUMENTS;
		}
		if (file->ef_structure == SC_FILE_EF_CYCLIC ||
		    file->ef_structure == SC_FILE_EF_LINEAR_FIXED)
		r = sc_asn1_put_tag(0x82, buf, 5, p, 8, &p);
	else
		r = sc_asn1_put_tag(0x82, buf, 1, p, 8, &p);
	} else if (file->type == SC_FILE_TYPE_DF) {
		buf[0] |= 0x38;
		r = sc_asn1_put_tag(0x82, buf, 1, p, 8, &p);
	} else
		return SC_ERROR_NOT_SUPPORTED;
	if (r != SC_SUCCESS)
		return r;
	/* set file id */
	buf[0] = (file->id >> 8) & 0xff;
	buf[1] = file->id        & 0xff;
	r = sc_asn1_put_tag(0x83, buf, 2, p, 8, &p);
	if (r != SC_SUCCESS)
		return r;
	/* set aid (for DF only) */
	if (file->type == SC_FILE_TYPE_DF && file->namelen != 0) {
		r = sc_asn1_put_tag(0x84, file->name, file->namelen, p, 20, &p);
		if (r != SC_SUCCESS)
			return r;
	}
	/* set proprietary file attributes */
	buf[0] = 0x00;		/* use default values */
	if (file->type == SC_FILE_TYPE_DF)
		r = sc_asn1_put_tag(0x85, buf, 1, p, 8, &p);
	else {
		buf[1] = 0x00;
		buf[2] = 0x00;
		r = sc_asn1_put_tag(0x85, buf, 1, p, 8, &p);
	}
	if (r != SC_SUCCESS)
		return r;
	/* set ACs  */
	len = 9;
	r = cardos_acl_to_bytes(card, file, buf, &len);
	if (r != SC_SUCCESS)
		return r;
	r = sc_asn1_put_tag(0x86, buf, len, p, 18, &p);
	if (r != SC_SUCCESS)
		return r;
	/* finally set the length of the FCP */
	out[1] = p - out - 2;

	*outlen = p - out;

	return SC_SUCCESS;
}