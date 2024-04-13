static const char *get_capability(struct client *client)
{
	struct imap_client *imap_client = (struct imap_client *)client;
	string_t *cap_str = t_str_new(256);
	bool explicit_capability = FALSE;

	if (*imap_client->set->imap_capability == '\0')
		str_append(cap_str, CAPABILITY_BANNER_STRING);
	else if (*imap_client->set->imap_capability != '+') {
		explicit_capability = TRUE;
		str_append(cap_str, imap_client->set->imap_capability);
	} else {
		str_append(cap_str, CAPABILITY_BANNER_STRING);
		str_append_c(cap_str, ' ');
		str_append(cap_str, imap_client->set->imap_capability + 1);
	}

	if (!explicit_capability) {
		if (imap_client->set->imap_literal_minus)
			str_append(cap_str, " LITERAL-");
		else
			str_append(cap_str, " LITERAL+");
	}

	if (client_is_tls_enabled(client) && !client->tls)
		str_append(cap_str, " STARTTLS");
	if (is_login_cmd_disabled(client))
		str_append(cap_str, " LOGINDISABLED");

	client_authenticate_get_capabilities(client, cap_str);
	return str_c(cap_str);
}