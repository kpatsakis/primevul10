iasecc_sdo_put_data(struct sc_card *card, struct iasecc_sdo_update *update)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu apdu;
	int ii, rv;

	LOG_FUNC_CALLED(ctx);
	if (update->magic != SC_CARDCTL_IASECC_SDO_MAGIC_PUT_DATA)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Invalid SDO update data");

	for(ii=0; update->fields[ii].tag && ii < IASECC_SDO_TAGS_UPDATE_MAX; ii++)   {
		unsigned char *encoded = NULL;
		int encoded_len;

		encoded_len = iasecc_sdo_encode_update_field(ctx, update->sdo_class, update->sdo_ref,
							&update->fields[ii], &encoded);
		sc_log(ctx, "iasecc_sdo_put_data() encode[%i]; tag %X; encoded_len %i", ii, update->fields[ii].tag, encoded_len);
		LOG_TEST_RET(ctx, encoded_len, "Cannot encode update data");

		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xDB, 0x3F, 0xFF);
		apdu.data = encoded;
		apdu.datalen = encoded_len;
		apdu.lc = encoded_len;
		apdu.flags |= SC_APDU_FLAGS_CHAINING;

		rv = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(ctx, rv, "APDU transmit failed");
		rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
		LOG_TEST_RET(ctx, rv, "SDO put data error");

		free(encoded);
	}

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}