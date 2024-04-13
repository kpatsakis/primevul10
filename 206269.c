static int cmd_starttls(struct imap_client *client,
			const struct imap_arg *args ATTR_UNUSED)
{
	client_cmd_starttls(&client->common);
	return 1;
}