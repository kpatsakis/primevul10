auth_client_request_get_server_pid(struct auth_client_request *request)
{
	return request->conn->server_pid;
}