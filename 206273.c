static bool client_invalid_command(struct imap_client *client)
{
	if (*client->cmd_tag == '\0')
		client->cmd_tag = "*";
	if (++client->common.bad_counter >= CLIENT_MAX_BAD_COMMANDS) {
		client_send_reply(&client->common, IMAP_CMD_REPLY_BYE,
				  "Too many invalid IMAP commands.");
		client_destroy(&client->common,
			       "Disconnected: Too many invalid commands");
		return FALSE;
	}
	client_send_reply(&client->common, IMAP_CMD_REPLY_BAD,
			  "Error in IMAP command received by server.");
	return TRUE;
}