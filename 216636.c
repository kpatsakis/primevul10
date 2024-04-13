static int cardos_acl_to_bytes(sc_card_t *card, const sc_file_t *file,
	u8 *buf, size_t *outlen)
{
	int       i, byte;
	const int *idx;

	if (buf == NULL || *outlen < 9)
		return SC_ERROR_INVALID_ARGUMENTS;

	idx = (file->type == SC_FILE_TYPE_DF) ?  df_acl : ef_acl;
	for (i = 0; i < 9; i++) {
		if (idx[i] < 0)
			byte = 0x00;
		else
			byte = acl_to_byte(sc_file_get_acl_entry(file, idx[i]));
		if (byte < 0) {
			sc_log(card->ctx,  "Invalid ACL\n");
			return SC_ERROR_INVALID_ARGUMENTS;
		}
		buf[i] = byte;
	}
	*outlen = 9;

	return SC_SUCCESS;
}