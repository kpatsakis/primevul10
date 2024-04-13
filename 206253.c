client_send_reply_raw(struct client *client,
		      const char *prefix, const char *resp_code,
		      const char *text, bool tagged)
{
	struct imap_client *imap_client = (struct imap_client *)client;

	T_BEGIN {
		string_t *line = t_str_new(256);

		if (tagged)
			str_append(line, imap_client->cmd_tag);
		else
			str_append_c(line, '*');
		str_append_c(line, ' ');
		str_append(line, prefix);
		str_append_c(line, ' ');
		if (resp_code != NULL)
			str_printfa(line, "[%s] ", resp_code);
		str_append(line, text);
		str_append(line, "\r\n");

		client_send_raw_data(client, str_data(line), str_len(line));
	} T_END;
}