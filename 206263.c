static int cmd_capability(struct imap_client *imap_client,
			  const struct imap_arg *args ATTR_UNUSED)
{
	struct client *client = &imap_client->common;

	/* Client is required to send CAPABILITY after STARTTLS, so the
	   capability resp-code workaround checks only pre-STARTTLS
	   CAPABILITY commands. */
	if (!client->starttls)
		imap_client->client_ignores_capability_resp_code = TRUE;
	client_send_raw(client, t_strconcat(
		"* CAPABILITY ", get_capability(client), "\r\n", NULL));
	client_send_reply(client, IMAP_CMD_REPLY_OK,
		"Pre-login capabilities listed, post-login capabilities have more.");
	return 1;
}