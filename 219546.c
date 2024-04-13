recv_hello(gnutls_session_t session, uint8_t * data, int datalen)
{
	int ret;

	if (session->security_parameters.entity == GNUTLS_CLIENT) {
		ret = read_server_hello(session, data, datalen);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
	} else {		/* Server side reading a client hello */

		ret = read_client_hello(session, data, datalen);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
	}

	return 0;
}