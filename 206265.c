bool client_handle_parser_error(struct imap_client *client,
				struct imap_parser *parser)
{
	const char *msg;
	enum imap_parser_error parse_error;

	msg = imap_parser_get_error(parser, &parse_error);
	switch (parse_error) {
	case IMAP_PARSE_ERROR_NONE:
		i_unreached();
	case IMAP_PARSE_ERROR_LITERAL_TOO_BIG:
		client_send_reply(&client->common,
				  IMAP_CMD_REPLY_BYE, msg);
		client_destroy(&client->common,
			       t_strconcat("Disconnected: ", msg, NULL));
		return FALSE;
	default:
		break;
	}

	client_send_reply(&client->common, IMAP_CMD_REPLY_BAD, msg);
	client->cmd_finished = TRUE;
	client->skip_line = TRUE;
	return TRUE;
}