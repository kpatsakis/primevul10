static void imap_login_init(void)
{
	imap_login_commands_init();
	imap_login_commands_register(imap_login_commands,
				     N_ELEMENTS(imap_login_commands));
}