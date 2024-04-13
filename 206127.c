const char *auth_client_request_get_cookie(struct auth_client_request *request)
{
	return request->conn->cookie;
}