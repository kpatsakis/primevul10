cardos_card_ctl(sc_card_t *card, unsigned long cmd, void *ptr)
{
	switch (cmd) {
	case SC_CARDCTL_CARDOS_PUT_DATA_FCI:
		break;
	case SC_CARDCTL_CARDOS_PUT_DATA_OCI:
		return cardos_put_data_oci(card,
			(struct sc_cardctl_cardos_obj_info *) ptr);
		break;
	case SC_CARDCTL_CARDOS_PUT_DATA_SECI:
		return cardos_put_data_seci(card,
			(struct sc_cardctl_cardos_obj_info *) ptr);
		break;
	case SC_CARDCTL_CARDOS_GENERATE_KEY:
		return cardos_generate_key(card,
			(struct sc_cardctl_cardos_genkey_info *) ptr);
	case  SC_CARDCTL_CARDOS_PASS_ALGO_FLAGS:
		return cardos_pass_algo_flags(card,
			(struct sc_cardctl_cardos_pass_algo_flags *) ptr);
	case SC_CARDCTL_LIFECYCLE_GET:
		return cardos_lifecycle_get(card, (int *) ptr);
	case SC_CARDCTL_LIFECYCLE_SET:
		return cardos_lifecycle_set(card, (int *) ptr);
	case SC_CARDCTL_GET_SERIALNR:
		return cardos_get_serialnr(card, (sc_serial_number_t *)ptr);
	}
	return SC_ERROR_NOT_SUPPORTED;
}