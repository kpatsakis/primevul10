static void imap_login_deinit(void)
{
	clients_destroy_all();
	imap_login_commands_deinit();
}