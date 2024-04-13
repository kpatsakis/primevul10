static int cardos_match_card(sc_card_t *card)
{
	unsigned char atr[SC_MAX_ATR_SIZE];
	int i;

	i = _sc_match_atr(card, cardos_atrs, &card->type);
	if (i < 0)
		return 0;

	memcpy(atr, card->atr.value, sizeof(atr));

	/* Do not change card type for CIE! */
	if (card->type == SC_CARD_TYPE_CARDOS_CIE_V1)
		return 1;
	if (card->type == SC_CARD_TYPE_CARDOS_M4_4)
		return 1;
	if (card->type == SC_CARD_TYPE_CARDOS_V5_0)
		return 1;
	if (card->type == SC_CARD_TYPE_CARDOS_V5_3)
		return 1;
	if (card->type == SC_CARD_TYPE_CARDOS_M4_2) {
		int rv;
		sc_apdu_t apdu;
		u8 rbuf[SC_MAX_APDU_BUFFER_SIZE];
		/* first check some additional ATR bytes */
		if ((atr[4] != 0xff && atr[4] != 0x02) ||
		    (atr[6] != 0x10 && atr[6] != 0x0a) ||
		    (atr[9] != 0x55 && atr[9] != 0x58))
			return 0;
		/* get the os version using GET DATA and compare it with
		 * version in the ATR */
		sc_log(card->ctx,  "checking cardos version ...");
		sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xca, 0x01, 0x82);
		apdu.resp = rbuf;
		apdu.resplen = sizeof(rbuf);
		apdu.le = 256;
		apdu.lc = 0;
		rv = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(card->ctx, rv, "APDU transmit failed");
		if (apdu.sw1 != 0x90 || apdu.sw2 != 0x00)
			return 0;
		if (apdu.resp[0] != atr[10] ||
		    apdu.resp[1] != atr[11])
			/* version mismatch */
			return 0;
		if (atr[11] <= 0x04) {
			sc_log(card->ctx,  "found cardos m4.01");
			card->type = SC_CARD_TYPE_CARDOS_M4_01;
		} else if (atr[11] == 0x08) {
			sc_log(card->ctx,  "found cardos v4.3b");
			card->type = SC_CARD_TYPE_CARDOS_M4_3;
		} else if (atr[11] == 0x09) {
			sc_log(card->ctx,  "found cardos v4.2b");
			card->type = SC_CARD_TYPE_CARDOS_M4_2B;
		} else if (atr[11] >= 0x0B) {
			sc_log(card->ctx,  "found cardos v4.2c or higher");
			card->type = SC_CARD_TYPE_CARDOS_M4_2C;
		} else {
			sc_log(card->ctx,  "found cardos m4.2");
		}
	}
	return 1;
}