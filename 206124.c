bool auth_client_request_is_aborted(struct auth_client_request *request)
{
	return request->callback == NULL;
}