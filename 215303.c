iasecc_sdo_get_tagged_data(struct sc_card *card, int sdo_tag, struct iasecc_sdo *sdo)
{
	struct sc_context *ctx = card->ctx;
	struct sc_apdu apdu;
	unsigned char sbuf[0x100];
	size_t offs = sizeof(sbuf) - 1;
	unsigned char rbuf[0x400];
	int rv;

	LOG_FUNC_CALLED(ctx);

	sbuf[offs--] = 0x80;
	sbuf[offs--] = sdo_tag & 0xFF;
	if ((sdo_tag >> 8) & 0xFF)
		sbuf[offs--] = (sdo_tag >> 8) & 0xFF;
	sbuf[offs] = sizeof(sbuf) - offs - 1;
	offs--;

	sbuf[offs--] = sdo->sdo_ref & 0x9F;
	sbuf[offs--] = sdo->sdo_class | IASECC_OBJECT_REF_LOCAL;
	sbuf[offs--] = IASECC_SDO_TAG_HEADER;

	sbuf[offs] = sizeof(sbuf) - offs - 1;
	offs--;
	sbuf[offs--] = IASECC_SDO_TEMPLATE_TAG;

	sbuf[offs] = sizeof(sbuf) - offs - 1;
	offs--;
	sbuf[offs] = 0x4D;

	sc_format_apdu(card, &apdu, SC_APDU_CASE_4_SHORT, 0xCB, 0x3F, 0xFF);
	apdu.data = sbuf + offs;
	apdu.datalen = sizeof(sbuf) - offs;
	apdu.lc = sizeof(sbuf) - offs;
	apdu.resp = rbuf;
	apdu.resplen = sizeof(rbuf);
	apdu.le = 0x100;

	rv = sc_transmit_apdu(card, &apdu);
	LOG_TEST_RET(ctx, rv, "APDU transmit failed");
	rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
	LOG_TEST_RET(ctx, rv, "SDO get data error");

	rv = iasecc_sdo_parse(card, apdu.resp, apdu.resplen, sdo);
	LOG_TEST_RET(ctx, rv, "cannot parse SDO data");

	LOG_FUNC_RETURN(ctx, rv);
}