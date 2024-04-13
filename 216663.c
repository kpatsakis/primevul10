static int cardos_create_file(sc_card_t *card, sc_file_t *file)
{
	int       r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	if (card->type == SC_CARD_TYPE_CARDOS_GENERIC ||
	    card->type == SC_CARD_TYPE_CARDOS_M4_01) {
		r = cardos_set_file_attributes(card, file);
		if (r != SC_SUCCESS)
			return r;
		return iso_ops->create_file(card, file);
	} else if (card->type == SC_CARD_TYPE_CARDOS_M4_2 ||
	           card->type == SC_CARD_TYPE_CARDOS_M4_3 ||
		   card->type == SC_CARD_TYPE_CARDOS_M4_2B ||
	           card->type == SC_CARD_TYPE_CARDOS_M4_2C ||
		   card->type == SC_CARD_TYPE_CARDOS_M4_4) {
		u8        sbuf[SC_MAX_APDU_BUFFER_SIZE];
		size_t    len = sizeof(sbuf);
		sc_apdu_t apdu;

		r = cardos_construct_fcp(card, file, sbuf, &len);
		if (r < 0) {
			sc_log(card->ctx,  "unable to create FCP");
			return r;
		}
	
		sc_format_apdu(card, &apdu, SC_APDU_CASE_3_SHORT, 0xE0, 0x00, 0x00);
		apdu.lc      = len;
		apdu.datalen = len;
		apdu.data    = sbuf;

		r = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

		return sc_check_sw(card, apdu.sw1, apdu.sw2);
	} else
		return SC_ERROR_NOT_SUPPORTED;
}