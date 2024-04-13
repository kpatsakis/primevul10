cardos_put_data_oci(sc_card_t *card,
			struct sc_cardctl_cardos_obj_info *args)
{
	sc_apdu_t	apdu;
	int		r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	memset(&apdu, 0, sizeof(apdu));
	apdu.cse = SC_APDU_CASE_3_SHORT;
	apdu.cla = 0x00;
	apdu.ins = 0xda;
	apdu.p1  = 0x01;
	apdu.p2  = 0x6e;
	apdu.lc  = args->len;
	apdu.data = args->data;
	apdu.datalen = args->len;

	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "Card returned error");

	LOG_FUNC_RETURN(card->ctx, r);
}