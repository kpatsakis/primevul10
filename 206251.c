imap_client_notify_disconnect(struct client *client,
			      enum client_disconnect_reason reason,
			      const char *text)
{
	if (reason == CLIENT_DISCONNECT_INTERNAL_ERROR) {
		client_send_reply_code(client, IMAP_CMD_REPLY_BYE,
				       IMAP_RESP_CODE_UNAVAILABLE, text);
	} else {
		client_send_reply_code(client, IMAP_CMD_REPLY_BYE, NULL, text);
	}
}