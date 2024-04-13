cardos_restore_security_env(sc_card_t *card, int se_num)
{
	sc_apdu_t apdu;
	int	r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	sc_format_apdu(card, &apdu, SC_APDU_CASE_1, 0x22, 0, se_num);
	apdu.p1 = (card->type == SC_CARD_TYPE_CARDOS_CIE_V1 ? 0xF3 : 0x03);

	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "Card returned error");

	LOG_FUNC_RETURN(card->ctx, r);
}