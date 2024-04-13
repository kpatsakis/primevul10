imap_client_notify_status(struct client *client, bool bad, const char *text)
{
	if (bad)
		client_send_reply_raw(client, "BAD", "ALERT", text, FALSE);
	else
		client_send_reply_raw(client, "OK", NULL, text, FALSE);
}