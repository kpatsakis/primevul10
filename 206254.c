void client_send_reply_code(struct client *client, enum imap_cmd_reply reply,
			    const char *resp_code, const char *text)
{
	const char *prefix = "NO";
	bool tagged = TRUE;

	switch (reply) {
	case IMAP_CMD_REPLY_OK:
		prefix = "OK";
		break;
	case IMAP_CMD_REPLY_NO:
		break;
	case IMAP_CMD_REPLY_BAD:
		prefix = "BAD";
		break;
	case IMAP_CMD_REPLY_BYE:
		prefix = "BYE";
		tagged = FALSE;
		break;
	}
	client_send_reply_raw(client, prefix, resp_code, text, tagged);
}