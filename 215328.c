iasecc_se_cache_info(struct sc_card *card, struct iasecc_se_info *se)
{
	struct iasecc_private_data *prv = (struct iasecc_private_data *) card->drv_data;
	struct sc_context *ctx = card->ctx;
	struct iasecc_se_info *se_info = NULL, *si = NULL;
	int rv;

	LOG_FUNC_CALLED(ctx);

	se_info = calloc(1, sizeof(struct iasecc_se_info));
	if (!se_info)
		LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "SE info allocation error");
	memcpy(se_info, se, sizeof(struct iasecc_se_info));

	if (card->cache.valid && card->cache.current_df)   {
		sc_file_dup(&se_info->df, card->cache.current_df);
		if (se_info->df == NULL)   {
			free(se_info);
			LOG_TEST_RET(ctx, SC_ERROR_OUT_OF_MEMORY, "Cannot duplicate current DF file");
		}
	}

	rv = iasecc_docp_copy(ctx, &se->docp, &se_info->docp);
	if (rv < 0)   {
		free(se_info->df);
		free(se_info);
		LOG_TEST_RET(ctx, rv, "Cannot make copy of DOCP");
	}

	if (!prv->se_info)   {
		prv->se_info = se_info;
	}
	else    {
		for (si = prv->se_info; si->next; si = si->next)
			;
		si->next = se_info;
	}

	LOG_FUNC_RETURN(ctx, rv);
}