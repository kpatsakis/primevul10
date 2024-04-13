void auth_client_request_server_input(struct auth_client_request *request,
				      enum auth_request_status status,
				      const char *const *args)
{
	const char *const *tmp, *base64_data = NULL;

	if (request->callback == NULL) {
		/* aborted already */
		return;
	}

	switch (status) {
	case AUTH_REQUEST_STATUS_OK:
		for (tmp = args; *tmp != NULL; tmp++) {
			if (strncmp(*tmp, "resp=", 5) == 0) {
				base64_data = *tmp + 5;
				break;
			}
		}
		break;
	case AUTH_REQUEST_STATUS_CONTINUE:
		base64_data = args[0];
		args = NULL;
		break;
	case AUTH_REQUEST_STATUS_FAIL:
	case AUTH_REQUEST_STATUS_INTERNAL_FAIL:
	case AUTH_REQUEST_STATUS_ABORT:
		break;
	}

	call_callback(request, status, base64_data, args);
	if (status != AUTH_REQUEST_STATUS_CONTINUE)
		pool_unref(&request->pool);
}