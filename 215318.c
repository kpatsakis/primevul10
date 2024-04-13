iasecc_logout(struct sc_card *card)
{
	struct sc_context *ctx = card->ctx;
	struct sc_path path;
	int rv;

	LOG_FUNC_CALLED(ctx);
	if (!card->ef_atr || !card->ef_atr->aid.len)
		return SC_SUCCESS;

	memset(&path, 0, sizeof(struct sc_path));
	path.type = SC_PATH_TYPE_DF_NAME;
	memcpy(path.value, card->ef_atr->aid.value, card->ef_atr->aid.len);
	path.len = card->ef_atr->aid.len;

	rv = iasecc_select_file(card, &path, NULL);
	sc_log(ctx, "Select ECC ROOT with the AID from EF.ATR: rv %i", rv);

	LOG_FUNC_RETURN(ctx, rv);
}