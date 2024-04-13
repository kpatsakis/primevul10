static int client_command_execute(struct imap_client *client, const char *cmd,
				  const struct imap_arg *args)
{
	struct imap_login_command *login_cmd;

	login_cmd = imap_login_command_lookup(cmd);
	if (login_cmd == NULL)
		return -2;
	return login_cmd->func(client, args);
}