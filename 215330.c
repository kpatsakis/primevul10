_iasecc_sm_read_binary(struct sc_card *card, unsigned int offs,
		unsigned char *buff, size_t count)
{
	struct sc_context *ctx = card->ctx;
	const struct sc_acl_entry *entry = NULL;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx,
	       "iasecc_sm_read_binary() card:%p offs:%i count:%"SC_FORMAT_LEN_SIZE_T"u ",
	       card, offs, count);
	if (offs > 0x7fff)
		LOG_TEST_RET(ctx, SC_ERROR_OFFSET_TOO_LARGE, "Invalid arguments");

	if (count == 0)
		return 0;

	sc_print_cache(card);

	if (card->cache.valid && card->cache.current_ef)   {
		entry = sc_file_get_acl_entry(card->cache.current_ef, SC_AC_OP_READ);
		if (!entry)
			LOG_TEST_RET(ctx, SC_ERROR_OBJECT_NOT_FOUND, "iasecc_sm_read() 'READ' ACL not present");

		sc_log(ctx, "READ method/reference %X/%X", entry->method, entry->key_ref);
		if ((entry->method == SC_AC_SCB) && (entry->key_ref & IASECC_SCB_METHOD_SM))   {
			unsigned char se_num = (entry->method == SC_AC_SCB) ? (entry->key_ref & IASECC_SCB_METHOD_MASK_REF) : 0;

			rv = iasecc_sm_read_binary(card, se_num, offs, buff, count);
			LOG_FUNC_RETURN(ctx, rv);
		}
	}

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}