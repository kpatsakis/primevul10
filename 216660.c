cardos_lifecycle_get(sc_card_t *card, int *mode)
{
	sc_apdu_t	apdu;
	u8 rbuf[SC_MAX_APDU_BUFFER_SIZE];
	int		r;

	SC_FUNC_CALLED(card->ctx, SC_LOG_DEBUG_VERBOSE);

	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xca, 0x01, 0x83);
	apdu.cla = 0x00;
	apdu.le = 256;
	apdu.resplen = sizeof(rbuf);
	apdu.resp = rbuf;

	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	r = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(card->ctx, r, "Card returned error");

	if (apdu.resplen < 1) {
		LOG_TEST_RET(card->ctx, r, "Lifecycle byte not in response");
	}

	r = SC_SUCCESS;
	switch (rbuf[0]) {
	case 0x10:
		*mode = SC_CARDCTRL_LIFECYCLE_USER;
		break;
	case 0x20:
		*mode = SC_CARDCTRL_LIFECYCLE_ADMIN;
		break;
	case 0x34: /* MANUFACTURING */
		*mode = SC_CARDCTRL_LIFECYCLE_OTHER;
		break;
	default:
		sc_log(card->ctx,  "Unknown lifecycle byte %d", rbuf[0]);
		r = SC_ERROR_INTERNAL;
	}

	LOG_FUNC_RETURN(card->ctx, r);
}