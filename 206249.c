static void imap_client_destroy(struct client *client)
{
	struct imap_client *imap_client = (struct imap_client *)client;

	i_free_and_null(imap_client->proxy_backend_capability);
	imap_parser_unref(&imap_client->parser);
}