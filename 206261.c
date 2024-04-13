static void imap_client_starttls(struct client *client)
{
	struct imap_client *imap_client = (struct imap_client *)client;

	imap_parser_unref(&imap_client->parser);
	imap_client->parser =
		imap_parser_create(imap_client->common.input,
				   imap_client->common.output,
				   IMAP_LOGIN_MAX_LINE_LENGTH);

	/* CRLF is lost from buffer when streams are reopened. */
	imap_client->skip_line = FALSE;
}