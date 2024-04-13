static int cmd_logout(struct imap_client *client,
		      const struct imap_arg *args ATTR_UNUSED)
{
	client_send_reply(&client->common, IMAP_CMD_REPLY_BYE, "Logging out");
	client_send_reply(&client->common, IMAP_CMD_REPLY_OK,
			  "Logout completed.");
	client_destroy(&client->common, "Aborted login");
	return 1;
}