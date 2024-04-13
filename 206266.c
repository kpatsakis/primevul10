imap_client_notify_starttls(struct client *client,
			    bool success, const char *text)
{
	if (success)
		client_send_reply(client, IMAP_CMD_REPLY_OK, text);
	else
		client_send_reply(client, IMAP_CMD_REPLY_BAD, text);
}