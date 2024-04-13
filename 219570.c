int _gnutls_set_server_random(gnutls_session_t session, uint8_t * rnd)
{
	int ret;

	if (rnd != NULL)
		memcpy(session->security_parameters.server_random, rnd,
		       GNUTLS_RANDOM_SIZE);
	else {
		/* no random given, we generate. */
		if (session->internals.sc_random_set != 0) {
			memcpy(session->security_parameters.server_random,
			       session->internals.
			       resumed_security_parameters.server_random,
			       GNUTLS_RANDOM_SIZE);
		} else {
			ret =
			    create_tls_random(session->
					      security_parameters.
					      server_random);
			if (ret < 0)
				return gnutls_assert_val(ret);
		}
	}
	return 0;
}