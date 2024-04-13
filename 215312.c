iasecc_emulate_fcp(struct sc_context *ctx, struct sc_apdu *apdu)
{
	unsigned char dummy_df_fcp[] = {
		0x62,0xFF,
			0x82,0x01,0x38,
			0x8A,0x01,0x05,
			0xA1,0x04,0x8C,0x02,0x02,0x00,
			0x84,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
				0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
	};

	LOG_FUNC_CALLED(ctx);

	if (apdu->p1 != 0x04)
		LOG_TEST_RET(ctx, SC_ERROR_NOT_SUPPORTED, "FCP emulation supported only for the DF-NAME selection type");
	if (apdu->datalen > 16)
		LOG_TEST_RET(ctx, SC_ERROR_INVALID_DATA, "Invalid DF-NAME length");
	if (apdu->resplen < apdu->datalen + 16)
		LOG_TEST_RET(ctx, SC_ERROR_BUFFER_TOO_SMALL, "not enough space for FCP data");

	memcpy(dummy_df_fcp + 16, apdu->data, apdu->datalen);
	dummy_df_fcp[15] = apdu->datalen;
	dummy_df_fcp[1] = apdu->datalen + 14;
	memcpy(apdu->resp, dummy_df_fcp, apdu->datalen + 16);

	LOG_FUNC_RETURN(ctx, SC_SUCCESS);
}