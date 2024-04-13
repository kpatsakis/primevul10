call_callback(struct auth_client_request *request,
	      enum auth_request_status status,
	      const char *data_base64,
	      const char *const *args)
{
	auth_request_callback_t *callback = request->callback;

	if (status != AUTH_REQUEST_STATUS_CONTINUE)
		request->callback = NULL;
	callback(request, status, data_base64, args, request->context);
}