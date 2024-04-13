static void imap_client_create(struct client *client, void **other_sets)
{
	struct imap_client *imap_client = (struct imap_client *)client;

	imap_client->set = other_sets[0];
	imap_client->parser =
		imap_parser_create(imap_client->common.input,
				   imap_client->common.output,
				   IMAP_LOGIN_MAX_LINE_LENGTH);
	if (imap_client->set->imap_literal_minus)
		imap_parser_enable_literal_minus(imap_client->parser);
	client->io = io_add_istream(client->input, client_input, client);
}