static int send_hello(gnutls_session_t session, int again)
{
	int ret;

	if (session->security_parameters.entity == GNUTLS_CLIENT) {
		ret = send_client_hello(session, again);

	} else {		/* SERVER */
		ret = send_server_hello(session, again);
	}

	return ret;
}