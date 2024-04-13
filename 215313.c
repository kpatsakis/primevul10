iasecc_sdo_generate(struct sc_card *card, struct iasecc_sdo *sdo)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_sdo_update update_pubkey;
	struct sc_apdu apdu;
	unsigned char scb, sbuf[5], rbuf[0x400], exponent[3] = {0x01, 0x00, 0x01};
	int offs = 0, rv = SC_ERROR_NOT_SUPPORTED;

	LOG_FUNC_CALLED(ctx);

	if (sdo->sdo_class != IASECC_SDO_CLASS_RSA_PRIVATE)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "For a moment, only RSA_PRIVATE class can be accepted for the SDO generation");

	if (sdo->docp.acls_contact.size == 0)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Bewildered ... there are no ACLs");

	scb = sdo->docp.scbs[IASECC_ACLS_RSAKEY_GENERATE];
	sc_log(ctx, "'generate RSA key' SCB 0x%X", scb);
	do   {
		unsigned all_conditions = scb & IASECC_SCB_METHOD_NEED_ALL ? 1 : 0;

		if (scb & IASECC_SCB_METHOD_USER_AUTH)
			if (!all_conditions)
				break;

		if (scb & IASECC_SCB_METHOD_EXT_AUTH)
			LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "Not yet");

		if (scb & IASECC_SCB_METHOD_SM)   {
			rv = iasecc_sm_rsa_generate(card, scb & IASECC_SCB_METHOD_MASK_REF, sdo);
                        LOG_FUNC_RETURN(ctx, rv);
		}
	} while(0);

	memset(&update_pubkey, 0, sizeof(update_pubkey));
	update_pubkey.magic = SC_CARDCTL_IASECC_SDO_MAGIC_PUT_DATA;
	update_pubkey.sdo_class = IASECC_SDO_CLASS_RSA_PUBLIC;
	update_pubkey.sdo_ref = sdo->sdo_ref;

	update_pubkey.fields[0].parent_tag = IASECC_SDO_PUBKEY_TAG;
	update_pubkey.fields[0].tag = IASECC_SDO_PUBKEY_TAG_E;
	update_pubkey.fields[0].value = exponent;
	update_pubkey.fields[0].size = sizeof(exponent);

	rv = iasecc_sdo_put_data(card, &update_pubkey);
	LOG_TEST_RET(ctx, rv, "iasecc_sdo_generate() update SDO public key failed");

	offs = 0;
	sbuf[offs++] = IASECC_SDO_TEMPLATE_TAG;
	sbuf[offs++] = 0x03;
	sbuf[offs++] = IASECC_SDO_TAG_HEADER;
	sbuf[offs++] = IASECC_SDO_CLASS_RSA_PRIVATE | IASECC_OBJECT_REF_LOCAL;
	sbuf[offs++] = sdo->sdo_ref & ~IASECC_OBJECT_REF_LOCAL;

	sc_format_apdu(card, &apdu, SC_APDU_CASE_4_SHORT, 0x47, 0x00, 0x00);
	apdu.data = sbuf;
	apdu.datalen = offs;
	apdu.lc = offs;
	apdu.resp = rbuf;
	apdu.resplen = sizeof(rbuf);
	apdu.le = 0x100;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "SDO get data error");

	LOG_FUNC_RETURN(ctx, rv);
}