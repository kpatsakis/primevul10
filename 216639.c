cardos_generate_key(sc_card_t *card,
		struct sc_cardctl_cardos_genkey_info *args)
{
	sc_apdu_t	apdu;
	u8		data[8];
	int		r;

	data[0] = 0x20;		/* store as PSO object */
	data[1] = args->key_id;
	data[2] = args->fid >> 8;
	data[3] = args->fid & 0xff;
	data[4] = 0;		/* additional Rabin Miller tests */
	data[5] = 0x10;		/* length difference between p, q (bits) */
	data[6] = 0;		/* default length of exponent, MSB */
	data[7] = 0x20;		/* default length of exponent, LSB */

	memset(&apdu, 0, sizeof(apdu));
	apdu.cse = SC_APDU_CASE_3_SHORT;
	apdu.cla = 0x00;
	apdu.ins = 0x46;
	apdu.p1  = 0x00;
	apdu.p2  = 0x00;
	apdu.data= data;
	apdu.datalen = apdu.lc = sizeof(data);

	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");
	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "GENERATE_KEY failed");

	return r;
}