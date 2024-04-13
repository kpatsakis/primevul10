iasecc_card_ctl(struct sc_card *card, unsigned long cmd, void *ptr)
{
	struct sc_context *ctx = card->ctx;
	struct iasecc_sdo *sdo = (struct iasecc_sdo *) ptr;

	switch (cmd) {
	case SC_CARDCTL_GET_SERIALNR:
		return iasecc_get_serialnr(card, (struct sc_serial_number *)ptr);
	case SC_CARDCTL_IASECC_SDO_CREATE:
		sc_log(ctx, "CMD SC_CARDCTL_IASECC_SDO_CREATE: sdo_class %X", sdo->sdo_class);
		return iasecc_sdo_create(card, (struct iasecc_sdo *) ptr);
	case SC_CARDCTL_IASECC_SDO_DELETE:
		sc_log(ctx, "CMD SC_CARDCTL_IASECC_SDO_DELETE: sdo_class %X", sdo->sdo_class);
		return iasecc_sdo_delete(card, (struct iasecc_sdo *) ptr);
	case SC_CARDCTL_IASECC_SDO_PUT_DATA:
		sc_log(ctx, "CMD SC_CARDCTL_IASECC_SDO_PUT_DATA: sdo_class %X", sdo->sdo_class);
		return iasecc_sdo_put_data(card, (struct iasecc_sdo_update *) ptr);
	case SC_CARDCTL_IASECC_SDO_KEY_RSA_PUT_DATA:
		sc_log(ctx, "CMD SC_CARDCTL_IASECC_SDO_KEY_RSA_PUT_DATA");
		return iasecc_sdo_key_rsa_put_data(card, (struct iasecc_sdo_rsa_update *) ptr);
	case SC_CARDCTL_IASECC_SDO_GET_DATA:
		sc_log(ctx, "CMD SC_CARDCTL_IASECC_SDO_GET_DATA: sdo_class %X", sdo->sdo_class);
		return iasecc_sdo_get_data(card, (struct iasecc_sdo *) ptr);
	case SC_CARDCTL_IASECC_SDO_GENERATE:
		sc_log(ctx, "CMD SC_CARDCTL_IASECC_SDO_GET_DATA: sdo_class %X", sdo->sdo_class);
		return iasecc_sdo_generate(card, (struct iasecc_sdo *) ptr);
	case SC_CARDCTL_GET_SE_INFO:
		sc_log(ctx, "CMD SC_CARDCTL_GET_SE_INFO: sdo_class %X", sdo->sdo_class);
		return iasecc_se_get_info(card, (struct iasecc_se_info *) ptr);
	case SC_CARDCTL_GET_CHV_REFERENCE_IN_SE:
		sc_log(ctx, "CMD SC_CARDCTL_GET_CHV_REFERENCE_IN_SE");
		return iasecc_get_chv_reference_from_se(card, (int *)ptr);
	case SC_CARDCTL_IASECC_GET_FREE_KEY_REFERENCE:
		sc_log(ctx, "CMD SC_CARDCTL_IASECC_GET_FREE_KEY_REFERENCE");
		return iasecc_get_free_reference(card, (struct iasecc_ctl_get_free_reference *)ptr);
	}
	return SC_ERROR_NOT_SUPPORTED;
}