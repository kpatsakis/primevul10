sc_awp_parse_df(struct sc_pkcs15_card *p15card, struct sc_pkcs15_df *df)
{
	struct sc_context *ctx = p15card->card->ctx;
	unsigned char *buf = NULL;
	size_t buf_len;
	int rv;

	LOG_FUNC_CALLED(ctx);
	if (df->type != SC_PKCS15_PRKDF && df->type != SC_PKCS15_DODF)
		LOG_FUNC_RETURN(ctx, SC_ERROR_NOT_SUPPORTED);

	if (df->enumerated)
		LOG_FUNC_RETURN(ctx, SC_SUCCESS);

	rv = sc_oberthur_read_file(p15card, AWP_OBJECTS_LIST_PRV, &buf, &buf_len, 1);
	LOG_TEST_RET(ctx, rv, "Parse DF: read private objects info failed");

	rv = sc_oberthur_parse_privateinfo(p15card, buf, buf_len, 0);

	if (buf)
		free(buf);

	if (rv == SC_ERROR_SECURITY_STATUS_NOT_SATISFIED)
		LOG_FUNC_RETURN(ctx, SC_SUCCESS);

	LOG_TEST_RET(ctx, rv, "Parse DF: private info parse error");
	df->enumerated = 1;

	LOG_FUNC_RETURN(ctx, rv);
}