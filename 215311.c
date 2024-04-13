_iasecc_sm_update_binary(struct sc_card *card, unsigned int offs,
		const unsigned char *buff, size_t count)
{
	struct sc_context *ctx = card->ctx;
	const struct sc_acl_entry *entry = NULL;
	int rv;

	if (count == 0)
		return SC_SUCCESS;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx,
	       "iasecc_sm_read_binary() card:%p offs:%i count:%"SC_FORMAT_LEN_SIZE_T"u ",
	       card, offs, count);
	sc_print_cache(card);

	if (card->cache.valid && card->cache.current_ef)   {
		entry = sc_file_get_acl_entry(card->cache.current_ef, SC_AC_OP_UPDATE);
		if (!entry)
			LOG_TEST_RET(ctx, SC_ERROR_OBJECT_NOT_FOUND, "iasecc_sm_update() 'UPDATE' ACL not present");

		sc_log(ctx, "UPDATE method/reference %X/%X", entry->method, entry->key_ref);
		if (entry->method == SC_AC_SCB && (entry->key_ref & IASECC_SCB_METHOD_SM))   {
			unsigned char se_num = entry->method == SC_AC_SCB ? entry->key_ref & IASECC_SCB_METHOD_MASK_REF : 0;

			rv = iasecc_sm_update_binary(card, se_num, offs, buff, count);
			LOG_FUNC_RETURN(ctx, rv);
		}
	}

	LOG_FUNC_RETURN(ctx, 0);
}