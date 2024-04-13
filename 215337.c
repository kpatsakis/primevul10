iasecc_read_binary(struct sc_card *card, unsigned int offs,
		unsigned char *buf, size_t count, unsigned long flags)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu apdu;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_log(ctx,
	       "iasecc_read_binary(card:%p) offs %i; count %"SC_FORMAT_LEN_SIZE_T"u",
	       card, offs, count);
	if (offs > 0x7fff) {
		sc_log(ctx, "invalid EF offset: 0x%X > 0x7FFF", offs);
		return SC_ERROR_OFFSET_TOO_LARGE;
	}

	sc_format_apdu(card, &apdu, SC_APDU_CASE_2_SHORT, 0xB0, (offs >> 8) & 0x7F, offs & 0xFF);
	apdu.le = count < 0x100 ? count : 0x100;
	apdu.resplen = count;
	apdu.resp = buf;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "iasecc_read_binary() failed");
	sc_log(ctx,
	       "iasecc_read_binary() apdu.resplen %"SC_FORMAT_LEN_SIZE_T"u",
	       apdu.resplen);

	if (apdu.resplen == IASECC_READ_BINARY_LENGTH_MAX && apdu.resplen < count)   {
		rv = iasecc_read_binary(card, offs + apdu.resplen, buf + apdu.resplen, count - apdu.resplen, flags);
		if (rv != SC_ERROR_WRONG_LENGTH)   {
			LOG_TEST_RET(ctx, rv, "iasecc_read_binary() read tail failed");
			apdu.resplen += rv;
		}
	}

	LOG_FUNC_RETURN(ctx, apdu.resplen);
}