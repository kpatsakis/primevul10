static void imap_client_notify_auth_ready(struct client *client)
{
	string_t *greet;

	greet = t_str_new(128);
	str_append(greet, "* OK ");
	str_printfa(greet, "[CAPABILITY %s] ", get_capability(client));
	str_append(greet, client->set->login_greeting);
	str_append(greet, "\r\n");

	client_send_raw(client, str_c(greet));

	client->banner_sent = TRUE;
}