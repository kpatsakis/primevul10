iasecc_sdo_create(struct sc_card *card, struct iasecc_sdo *sdo)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu apdu;
	unsigned char *data = NULL, sdo_class = sdo->sdo_class;
	struct iasecc_sdo_update update;
	struct iasecc_extended_tlv *field = NULL;
	int rv = SC_ERROR_NOT_SUPPORTED, data_len;

	LOG_FUNC_CALLED(ctx);
	if (sdo->magic != SC_CARDCTL_IASECC_SDO_MAGIC)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Invalid SDO data");

	sc_log(ctx, "iasecc_sdo_create(card:%p) %02X%02X%02X", card,
			IASECC_SDO_TAG_HEADER, sdo->sdo_class | 0x80, sdo->sdo_ref);

	data_len = iasecc_sdo_encode_create(ctx, sdo, &data);
	LOG_TEST_RET(ctx, data_len, "iasecc_sdo_create() cannot encode SDO create data");
	sc_log(ctx, "iasecc_sdo_create() create data(%i):%s", data_len, sc_dump_hex(data, data_len));

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xDB, 0x3F, 0xFF);
	apdu.data = data;
	apdu.datalen = data_len;
	apdu.lc = data_len;
	apdu.flags |= SC_APDU_FLAGS_CHAINING;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "iasecc_sdo_create() SDO put data error");

	memset(&update, 0, sizeof(update));
	update.magic = SC_CARDCTL_IASECC_SDO_MAGIC_PUT_DATA;
	update.sdo_class = sdo->sdo_class;
	update.sdo_ref = sdo->sdo_ref;

	if (sdo_class == IASECC_SDO_CLASS_RSA_PRIVATE)   {
		update.fields[0] = sdo->data.prv_key.compulsory;
		update.fields[0].parent_tag = IASECC_SDO_PRVKEY_TAG;
		field = &sdo->data.prv_key.compulsory;
	}
	else if (sdo_class == IASECC_SDO_CLASS_RSA_PUBLIC)   {
		update.fields[0] = sdo->data.pub_key.compulsory;
		update.fields[0].parent_tag = IASECC_SDO_PUBKEY_TAG;
		field = &sdo->data.pub_key.compulsory;
	}
	else if (sdo_class == IASECC_SDO_CLASS_KEYSET)   {
		update.fields[0] = sdo->data.keyset.compulsory;
		update.fields[0].parent_tag = IASECC_SDO_KEYSET_TAG;
		field = &sdo->data.keyset.compulsory;
	}

	if (update.fields[0].value && !update.fields[0].on_card)   {
		rv = iasecc_sdo_put_data(card, &update);
		LOG_TEST_RET(ctx, rv, "failed to update 'Compulsory usage' data");

		if (field)
			field->on_card = 1;
	}

	free(data);
	LOG_FUNC_RETURN(ctx, rv);
}