iasecc_get_chv_reference_from_se(struct sc_card *card, int *se_reference)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_se_info se;
	struct sc_crt crt;
	int rv;

	LOG_FUNC_CALLED(ctx);

	if (!se_reference)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_ARGUMENTS, "Invalid arguments");

	memset(&se, 0, sizeof(se));
	se.reference = *se_reference;

	rv = iasecc_se_get_info(card, &se);
	LOG_TEST_RET(ctx, rv, "get SE info error");

	memset(&crt, 0, sizeof(crt));
	crt.tag = IASECC_CRT_TAG_AT;
	crt.usage = IASECC_UQB_AT_USER_PASSWORD;

	rv = iasecc_se_get_crt(card, &se, &crt);
	LOG_TEST_RET(ctx, rv, "Cannot get 'USER PASSWORD' authentication template");

	sc_file_free(se.df);
	LOG_FUNC_RETURN(ctx, crt.refs[0]);
}