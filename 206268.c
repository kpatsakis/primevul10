static int cmd_enable(struct imap_client *client,
		      const struct imap_arg *args ATTR_UNUSED)
{
	client_send_raw(&client->common, "* ENABLED\r\n");
	client_send_reply(&client->common, IMAP_CMD_REPLY_OK,
			  "ENABLE ignored in non-authenticated state.");
	return 1;
}