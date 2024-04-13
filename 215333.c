iasecc_se_get_info(struct sc_card *card, struct iasecc_se_info *se)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu apdu;
	unsigned char rbuf[0x100];
	unsigned char sbuf_iasecc[10] = {
		0x4D, 0x08, IASECC_SDO_TEMPLATE_TAG, 0x06,
		IASECC_SDO_TAG_HEADER, IASECC_SDO_CLASS_SE | IASECC_OBJECT_REF_LOCAL,
		se->reference & 0x3F,
		0x02, IASECC_SDO_CLASS_SE, 0x80
	};
	int rv;

	LOG_FUNC_CALLED(ctx);

	if (se->reference > IASECC_SE_REF_MAX)
                LOG_FUNC_RETURN(ctx, SC_ERROR_INVALID_ARGUMENTS);

	rv = iasecc_se_get_info_from_cache(card, se);
	if (rv == SC_ERROR_OBJECT_NOT_FOUND)   {
		sc_log(ctx, "No SE#%X info in cache, try to use 'GET DATA'", se->reference);

		sc_format_apdu(card, &apdu, SC_APDU_CASE_4_SHORT, 0xCB, 0x3F, 0xFF);
		apdu.data = sbuf_iasecc;
		apdu.datalen = sizeof(sbuf_iasecc);
		apdu.lc = apdu.datalen;
		apdu.resp = rbuf;
		apdu.resplen = sizeof(rbuf);
		apdu.le = sizeof(rbuf);

		rv = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(ctx, rv, "APDU transmit failed");
		rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
		LOG_TEST_RET(ctx, rv, "get SE data  error");

		rv = iasecc_se_parse(card, apdu.resp, apdu.resplen, se);
		LOG_TEST_RET(ctx, rv, "cannot parse SE data");

		rv = iasecc_se_cache_info(card, se);
		LOG_TEST_RET(ctx, rv, "failed to put SE data into cache");
	}

	LOG_FUNC_RETURN(ctx, rv);
}