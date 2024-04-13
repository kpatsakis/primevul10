iasecc_mi_match(struct sc_card *card)
{
	struct sc_context *ctx = card->ctx;
	unsigned char resp[0x100];
	size_t resp_len;
	int rv = 0;

	LOG_FUNC_CALLED(ctx);

	resp_len = sizeof(resp);
	rv = iasecc_select_aid(card, &MIIASECC_AID, resp, &resp_len);
	LOG_TEST_RET(ctx, rv, "IASECC: failed to select MI IAS/ECC applet");

	if (!card->ef_atr)
		card->ef_atr = calloc(1, sizeof(struct sc_ef_atr));
	if (!card->ef_atr)
		LOG_FUNC_RETURN(ctx, SC_ERROR_OUT_OF_MEMORY);

	memcpy(card->ef_atr->aid.value, MIIASECC_AID.value, MIIASECC_AID.len);
	card->ef_atr->aid.len = MIIASECC_AID.len;

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}