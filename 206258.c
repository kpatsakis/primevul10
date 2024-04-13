static void imap_client_input(struct client *client)
{
	struct imap_client *imap_client = (struct imap_client *)client;

	if (!client_read(client))
		return;

	client_ref(client);
	o_stream_cork(imap_client->common.output);
	for (;;) {
		if (!auth_client_is_connected(auth_client)) {
			/* we're not currently connected to auth process -
			   don't allow any commands */
			client_notify_status(client, FALSE,
					     AUTH_SERVER_WAITING_MSG);
			timeout_remove(&client->to_auth_waiting);

			client->input_blocked = TRUE;
			break;
		} else {
			if (!client_handle_input(imap_client))
				break;
		}
	}
	o_stream_uncork(imap_client->common.output);
	client_unref(&client);
}