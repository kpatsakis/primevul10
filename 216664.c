static struct sc_card_driver * sc_get_driver(void)
{
	if (iso_ops == NULL)
		iso_ops = sc_get_iso7816_driver()->ops;
	cardos_ops = *iso_ops;
	cardos_ops.match_card = cardos_match_card;
	cardos_ops.init = cardos_init;
	cardos_ops.finish = cardos_finish;
	cardos_ops.select_file = cardos_select_file;
	cardos_ops.create_file = cardos_create_file;
	cardos_ops.set_security_env = cardos_set_security_env;
	cardos_ops.restore_security_env = cardos_restore_security_env;
	cardos_ops.compute_signature = cardos_compute_signature;
	cardos_ops.decipher = cardos_decipher;

	cardos_ops.list_files = cardos_list_files;
	cardos_ops.check_sw = cardos_check_sw;
	cardos_ops.card_ctl = cardos_card_ctl;
	cardos_ops.pin_cmd = cardos_pin_cmd;
	cardos_ops.logout  = cardos_logout;

	return &cardos_drv;
}