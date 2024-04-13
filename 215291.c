iasecc_create_file(struct sc_card *card, struct sc_file *file)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu apdu;
	const struct sc_acl_entry *entry = NULL;
	unsigned char sbuf[0x100];
	size_t sbuf_len;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_print_cache(card);

	if (file->type != SC_FILE_TYPE_WORKING_EF)
		LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Creation of the file with of this type is not supported");

	sbuf_len = iasecc_fcp_encode(card, file, sbuf + 2, sizeof(sbuf)-2);
	LOG_TEST_RET(ctx, sbuf_len, "FCP encode error");

	sbuf[0] = IASECC_FCP_TAG;
	sbuf[1] = sbuf_len;

	if (card->cache.valid && card->cache.current_df)   {
		entry = sc_file_get_acl_entry(card->cache.current_df, SC_AC_OP_CREATE);
		if (!entry)
			LOG_TEST_RET(ctx, SC_ERROR_OBJECT_NOT_FOUND, "iasecc_create_file() 'CREATE' ACL not present");

		sc_log(ctx, "iasecc_create_file() 'CREATE' method/reference %X/%X", entry->method, entry->key_ref);
		sc_log(ctx, "iasecc_create_file() create data: '%s'", sc_dump_hex(sbuf, sbuf_len + 2));
		if (entry->method == SC_AC_SCB && (entry->key_ref & IASECC_SCB_METHOD_SM))   {
                        rv = iasecc_sm_create_file(card, entry->key_ref & IASECC_SCB_METHOD_MASK_REF, sbuf, sbuf_len + 2);
                        LOG_TEST_RET(ctx, rv, "iasecc_create_file() SM create file error");

                        rv = iasecc_select_file(card, &file->path, NULL);
                        LOG_FUNC_RETURN(ctx, rv);

		}
	}

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xE0, 0, 0);
	apdu.data = sbuf;
	apdu.datalen = sbuf_len + 2;
	apdu.lc = sbuf_len + 2;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "iasecc_create_file() create file error");

	rv = iasecc_select_file(card, &file->path, NULL);
	LOG_TEST_RET(ctx, rv, "Cannot select newly created file");

	LOG_FUNC_RETURN(ctx, rv);
}