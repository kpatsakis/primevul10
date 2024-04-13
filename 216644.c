do_compute_signature(sc_card_t *card, const u8 *data, size_t datalen,
		     u8 *out, size_t outlen)
{
	/* cardos_data_t* priv = (cardos_data_t*)card->drv_dataa */;
	int r;
	sc_apdu_t apdu;

	/* INS: 0x2A  PERFORM SECURITY OPERATION
	 * P1:  0x9E  Resp: Digital Signature
	 * P2:  0x9A  Cmd: Input for Digital Signature */
	sc_format_apdu(card, &apdu, SC_APDU_CASE_4, 0x2A, 0x9E, 0x9A);
	apdu.resp    = out;
	apdu.le      = outlen;
	apdu.resplen = outlen;

	apdu.data    = data;
	apdu.lc      = datalen;
	apdu.datalen = datalen;
	fixup_transceive_length(card, &apdu);
	r = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(card->ctx, r, "APDU transmit failed");

	if (apdu.sw1 == 0x90 && apdu.sw2 == 0x00)
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, apdu.resplen);
	else
		SC_FUNC_RETURN(card->ctx, SC_LOG_DEBUG_VERBOSE, sc_check_sw(card, apdu.sw1, apdu.sw2));
}