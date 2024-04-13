int _gnutls_set_client_random(gnutls_session_t session, uint8_t * rnd)
{
	int ret;

	if (rnd != NULL)
		memcpy(session->security_parameters.client_random, rnd,
		       GNUTLS_RANDOM_SIZE);
	else {
		/* no random given, we generate. */
		if (session->internals.sc_random_set != 0) {
			memcpy(session->security_parameters.client_random,
			       session->internals.
			       resumed_security_parameters.client_random,
			       GNUTLS_RANDOM_SIZE);
		} else {
			ret =
			    create_tls_random(session->
					      security_parameters.
					      client_random);
			if (ret < 0)
				return gnutls_assert_val(ret);
		}
	}
	return 0;
}