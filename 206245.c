void client_send_reply(struct client *client, enum imap_cmd_reply reply,
		       const char *text)
{
	client_send_reply_code(client, reply, NULL, text);
}