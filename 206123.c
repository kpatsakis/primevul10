void auth_client_request_continue(struct auth_client_request *request,
                                  const char *data_base64)
{
	struct const_iovec iov[3];
	const char *prefix;

	prefix = t_strdup_printf("CONT\t%u\t", request->id);

	iov[0].iov_base = prefix;
	iov[0].iov_len = strlen(prefix);
	iov[1].iov_base = data_base64;
	iov[1].iov_len = strlen(data_base64);
	iov[2].iov_base = "\n";
	iov[2].iov_len = 1;

	if (o_stream_sendv(request->conn->output, iov, 3) < 0)
		i_error("Error sending continue request to auth server: %m");
}