static int client_parse_command(struct imap_client *client,
				const struct imap_arg **args_r)
{
	switch (imap_parser_read_args(client->parser, 0, 0, args_r)) {
	case -1:
		/* error */
		if (!client_handle_parser_error(client, client->parser)) {
			/* client destroyed */
			return 0;
		}
		return -1;
	case -2:
		/* not enough data */
		return 0;
	default:
		/* we read the entire line - skip over the CRLF */
		if (!client_skip_line(client))
			i_unreached();
		return 1;
	}
}