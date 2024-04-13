cardos_logout(sc_card_t *card)
{
	if (card->type == SC_CARD_TYPE_CARDOS_M4_01
		   	|| card->type == SC_CARD_TYPE_CARDOS_M4_2
		   	|| card->type == SC_CARD_TYPE_CARDOS_M4_2B
		   	|| card->type == SC_CARD_TYPE_CARDOS_M4_2C
		   	|| card->type == SC_CARD_TYPE_CARDOS_M4_3
		   	|| card->type == SC_CARD_TYPE_CARDOS_M4_4
			|| card->type == SC_CARD_TYPE_CARDOS_V5_0
			|| card->type == SC_CARD_TYPE_CARDOS_V5_3) {
		sc_apdu_t apdu;
		int       r;
		sc_path_t path;

		sc_format_path("3F00", &path);
		r = sc_select_file(card, &path, NULL);
		if (r != SC_SUCCESS)
			return r;

		sc_format_apdu(card, &apdu, SC_APDU_CASE_1, 0xEA, 0x00, 0x00);
		apdu.cla = 0x80;

		r = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

		return sc_check_sw(card, apdu.sw1, apdu.sw2);
	} else
		return SC_ERROR_NOT_SUPPORTED;
}