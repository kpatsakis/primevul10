iasecc_sdo_delete(struct sc_card *card, struct iasecc_sdo *sdo)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu apdu;
	unsigned char data[6] = {
		0x70, 0x04, 0xBF, 0xFF, 0xFF, 0x00
	};
	int rv;

	LOG_FUNC_CALLED(ctx);
	if (sdo->magic != SC_CARDCTL_IASECC_SDO_MAGIC)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Invalid SDO data");

	data[2] = IASECC_SDO_TAG_HEADER;
	data[3] = sdo->sdo_class | 0x80;
	data[4] = sdo->sdo_ref;
	sc_log(ctx, "delete SDO %02X%02X%02X", data[2], data[3], data[4]);

	sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xDB, 0x3F, 0xFF);
	apdu.data = data;
	apdu.datalen = sizeof(data);
	apdu.lc = sizeof(data);
	apdu.flags |= SC_APDU_FLAGS_CHAINING;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "delete SDO error");

	LOG_FUNC_RETURN(ctx, rv);
}