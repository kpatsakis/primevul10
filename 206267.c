static void imap_login_preinit(void)
{
	login_set_roots = imap_login_setting_roots;
}