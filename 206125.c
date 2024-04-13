auth_client_request_new(struct auth_client *client,
			const struct auth_request_info *request_info,
			auth_request_callback_t *callback, void *context)
{
	struct auth_client_request *request;
	pool_t pool;

	pool = pool_alloconly_create("auth client request", 512);
	request = p_new(pool, struct auth_client_request, 1);
	request->pool = pool;
	request->conn = client->conn;

	request->callback = callback;
	request->context = context;

	request->id =
		auth_server_connection_add_request(request->conn, request);
	request->created = ioloop_time;
	T_BEGIN {
		auth_server_send_new_request(request->conn, request, request_info);
	} T_END;
	return request;
}