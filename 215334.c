iasecc_se_get_info_from_cache(struct sc_card *card, struct iasecc_se_info *se)
{
	struct iasecc_private_data *prv = (struct iasecc_private_data *) card->drv_data;
	struct sc_context *ctx = card->ctx;
	struct iasecc_se_info *si = NULL;
	int rv;

	LOG_FUNC_CALLED(ctx);

	for(si = prv->se_info; si; si = si->next)   {
		if (si->reference != se->reference)
			continue;
		if (!(card->cache.valid && card->cache.current_df) && si->df)
			continue;
		if (card->cache.valid && card->cache.current_df && !si->df)
			continue;
		if (card->cache.valid && card->cache.current_df && si->df)
			if (memcmp(&card->cache.current_df->path, &si->df->path, sizeof(struct sc_path)))
				continue;
		break;
	}

	if (!si)
		return SC_ERROR_OBJECT_NOT_FOUND;

	memcpy(se, si, sizeof(struct iasecc_se_info));

	if (si->df)   {
		sc_file_dup(&se->df, si->df);
		if (se->df == NULL)
			LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "Cannot duplicate current DF file");
	}

	rv = iasecc_docp_copy(ctx, &si->docp, &se->docp);
	LOG_TEST_RET(ctx, rv, "Cannot make copy of DOCP");

	LOG_FUNC_RETURN(ctx, rv);
}