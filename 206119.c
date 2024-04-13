void auth_client_send_cancel(struct auth_client *client, unsigned int id)
{
	const char *str = t_strdup_printf("CANCEL\t%u\n", id);

	if (o_stream_send_str(client->conn->output, str) < 0)
		i_error("Error sending request to auth server: %m");
}