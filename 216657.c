cardos_lifecycle_set(sc_card_t *card, int *mode)
{
	sc_apdu_t	apdu;
	int		r;

	int current;
	int target;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	target = *mode;

	r = cardos_lifecycle_get(card, &current);
	
	if (r != SC_SUCCESS)
		return r;

	if (current == target || current == SC_CARDCTRL_LIFECYCLE_OTHER)
		return SC_SUCCESS;

	sc_format_apdu(card, &apdu, SC_APDU_CASE_1, 0x10, 0, 0);
	apdu.cla = 0x80;
	apdu.le = 0;
	apdu.resplen = 0;
	apdu.resp = NULL;

	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "Card returned error");

	LOG_FUNC_RETURN(card->ctx, r);
}