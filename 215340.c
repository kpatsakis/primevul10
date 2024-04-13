iasecc_se_at_to_chv_reference(struct sc_card *card, unsigned reference,
		unsigned *chv_reference)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_se_info se;
	struct sc_crt crt;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx, "SE reference %i", reference);

	if (reference > IASECC_SE_REF_MAX)
		LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);

	memset(&se, 0, sizeof(se));
	se.reference = reference;

	rv = iasecc_se_get_info(card, &se);
	LOG_TEST_RET(ctx, rv, "SDO get data error");

	memset(&crt, 0, sizeof(crt));
	crt.tag = IASECC_CRT_TAG_AT;
	crt.usage = IASECC_UQB_AT_USER_PASSWORD;

	rv = iasecc_se_get_crt(card, &se, &crt);
	LOG_TEST_RET(ctx, rv, "no authentication template for USER PASSWORD");

	if (chv_reference)
		*chv_reference = crt.refs[0];

	sc_file_free(se.df);

	LOG_FUNC_RETURN(ctx, rv);
}