iasecc_init_gemalto(struct sc_card *card)
{
	struct sc_context *ctx = card->ctx;
	struct sc_path path;
	unsigned int flags;
	int rv = 0;

	LOG_FUNC_CALLED(ctx);

	flags = IASECC_CARD_DEFAULT_FLAGS;

	card->caps = IASECC_CARD_DEFAULT_CAPS;

	sc_format_path("3F00", &path);
	if (SC_SUCCESS != sc_select_file(card, &path, NULL)) {
		/* Result ignored*/
		sc_log(card->ctx, "Warning, MF select failed");
	}

	rv = iasecc_parse_ef_atr(card);
	sc_log(ctx, "rv %i", rv);
	if (rv == SC_ERROR_FILE_NOT_FOUND)   {
		sc_log(ctx, "Select MF");
		rv = iasecc_select_mf(card, NULL);
		sc_log(ctx, "rv %i", rv);
		LOG_TEST_RET(ctx, rv, "MF selection error");

		rv = iasecc_parse_ef_atr(card);
		sc_log(ctx, "rv %i", rv);
	}
	sc_log(ctx, "rv %i", rv);
	LOG_TEST_RET(ctx, rv, "Cannot read/parse EF.ATR");

	_sc_card_add_rsa_alg(card, 1024, flags, 0x10001);
	_sc_card_add_rsa_alg(card, 2048, flags, 0x10001);

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}