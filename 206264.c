static int cmd_noop(struct imap_client *client,
		    const struct imap_arg *args ATTR_UNUSED)
{
	client_send_reply(&client->common, IMAP_CMD_REPLY_OK,
			  "NOOP completed.");
	return 1;
}