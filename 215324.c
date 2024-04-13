iasecc_oberthur_match(struct sc_card *card)
{
	struct sc_context *ctx = card->ctx;
	unsigned char *hist = card->reader->atr_info.hist_bytes;

	LOG_FUNC_CALLED(ctx);

	if (*hist != 0x80 || ((*(hist+1)&0xF0) != 0xF0))
		LOG_FUNC_RETURN(ctx, SC_ERROR_OBJECT_NOT_FOUND);

	sc_log_hex(ctx, "AID in historical_bytes", hist + 2, *(hist+1) & 0x0F);

	if (memcmp(hist + 2, OberthurIASECC_AID.value, *(hist+1) & 0x0F))
		LOG_FUNC_RETURN(ctx, SC_ERROR_RECORD_NOT_FOUND);

	if (!card->ef_atr)
		card->ef_atr = calloc(1, sizeof(struct sc_ef_atr));
	if (!card->ef_atr)
		LOG_FUNC_RETURN(ctx, SC_ERROR_OUT_OF_MEMORY);

	memcpy(card->ef_atr->aid.value, OberthurIASECC_AID.value, OberthurIASECC_AID.len);
	card->ef_atr->aid.len = OberthurIASECC_AID.len;

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}