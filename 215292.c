static int iasecc_parse_ef_atr(struct sc_card *card)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_private_data *pdata = (struct iasecc_private_data *) card->drv_data;
	struct iasecc_version *version = &pdata->version;
	struct iasecc_io_buffer_sizes *sizes = &pdata->max_sizes;
	int rv;

	LOG_FUNC_CALLED(ctx);
	rv = sc_parse_ef_atr(card);
	LOG_TEST_RET(ctx, rv, "MF selection error");

	if (card->ef_atr->pre_issuing_len < 4)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Invalid pre-issuing data");

	version->ic_manufacturer =	card->ef_atr->pre_issuing[0];
	version->ic_type =		card->ef_atr->pre_issuing[1];
	version->os_version =		card->ef_atr->pre_issuing[2];
	version->iasecc_version =	card->ef_atr->pre_issuing[3];
	sc_log(ctx, "EF.ATR: IC manufacturer/type %X/%X, OS/IasEcc versions %X/%X",
		version->ic_manufacturer, version->ic_type, version->os_version, version->iasecc_version);

	if (card->ef_atr->issuer_data_len < 16)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Invalid issuer data");

	sizes->send =	 card->ef_atr->issuer_data[2] * 0x100 + card->ef_atr->issuer_data[3];
	sizes->send_sc = card->ef_atr->issuer_data[6] * 0x100 + card->ef_atr->issuer_data[7];
	sizes->recv =	 card->ef_atr->issuer_data[10] * 0x100 + card->ef_atr->issuer_data[11];
	sizes->recv_sc = card->ef_atr->issuer_data[14] * 0x100 + card->ef_atr->issuer_data[15];

	card->max_send_size = sizes->send;
	card->max_recv_size = sizes->recv;

	/* Most of the card producers interpret 'send' values as "maximum APDU data size".
	 * Oberthur strictly follows specification and interpret these values as "maximum APDU command size".
	 * Here we need 'data size'.
	 */
	if (card->max_send_size > 0xFF)
		card->max_send_size -= 5;

	sc_log(ctx,
	       "EF.ATR: max send/recv sizes %"SC_FORMAT_LEN_SIZE_T"X/%"SC_FORMAT_LEN_SIZE_T"X",
	       card->max_send_size, card->max_recv_size);

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}