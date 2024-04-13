static int cardos_get_serialnr(sc_card_t *card, sc_serial_number_t *serial)
{
	int r;
	sc_apdu_t apdu;
	u8  rbuf[SC_MAX_APDU_BUFFER_SIZE];

	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xca, 0x01, 0x81);
	apdu.resp = rbuf;
	apdu.resplen = sizeof(rbuf);
	apdu.le   = 256;
	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r,  "APDU transmit failed");
	if (apdu.sw1 != 0x90 || apdu.sw2 != 0x00)
		return SC_ERROR_INTERNAL;
	if ((apdu.resplen == 8) && (card->type == SC_CARD_TYPE_CARDOS_V5_0 || card->type == SC_CARD_TYPE_CARDOS_V5_3)) {
		/* cache serial number */
		memcpy(card->serialnr.value, rbuf, 8);
		card->serialnr.len = 8;
	} else if (apdu.resplen == 32) {
		/* cache serial number */
		memcpy(card->serialnr.value, &rbuf[10], 6);
		card->serialnr.len = 6;
	} else {
		sc_log(card->ctx,  "unexpected response to GET DATA serial"
				" number\n");
		return SC_ERROR_INTERNAL;
	}
	/* copy and return serial number */
	memcpy(serial, &card->serialnr, sizeof(*serial));
	return SC_SUCCESS;
}