iasecc_sdo_get_data(struct sc_card *card, struct iasecc_sdo *sdo)
{
	struct sc_context *ctx = card->ctx;
	int rv, sdo_tag;

	LOG_FUNC_CALLED(ctx);

	sdo_tag = iasecc_sdo_tag_from_class(sdo->sdo_class);

	rv = iasecc_sdo_get_tagged_data(card, sdo_tag, sdo);
	/* When there is no public data 'GET DATA' returns error */
	if (rv != SC_ERROR_INCORRECT_PARAMETERS)
		LOG_TEST_RET(ctx, rv, "cannot parse ECC SDO data");

	rv = iasecc_sdo_get_tagged_data(card, IASECC_DOCP_TAG, sdo);
	LOG_TEST_RET(ctx, rv, "cannot parse ECC DOCP data");

	LOG_FUNC_RETURN(ctx, rv);
}