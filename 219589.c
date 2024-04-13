client_check_if_resuming(gnutls_session_t session,
				 uint8_t * session_id, int session_id_len)
{
	char buf[2 * GNUTLS_MAX_SESSION_ID_SIZE + 1];

	_gnutls_handshake_log("HSK[%p]: SessionID length: %d\n", session,
			      session_id_len);
	_gnutls_handshake_log("HSK[%p]: SessionID: %s\n", session,
			      _gnutls_bin2hex(session_id, session_id_len,
					      buf, sizeof(buf), NULL));

	if ((session->internals.resumption_requested != 0 ||
	     session->internals.premaster_set != 0) &&
	    session_id_len > 0 &&
	    session->internals.resumed_security_parameters.
	    session_id_size == session_id_len
	    && memcmp(session_id,
		      session->internals.resumed_security_parameters.
		      session_id, session_id_len) == 0) {
		/* resume session */
		memcpy(session->internals.resumed_security_parameters.
		       server_random,
		       session->security_parameters.server_random,
		       GNUTLS_RANDOM_SIZE);
		memcpy(session->internals.resumed_security_parameters.
		       client_random,
		       session->security_parameters.client_random,
		       GNUTLS_RANDOM_SIZE);

		memcpy(session->security_parameters.cipher_suite,
			session->internals.resumed_security_parameters.cipher_suite, 2);
                session->security_parameters.compression_method =
		        session->internals.resumed_security_parameters.compression_method;

		_gnutls_epoch_set_cipher_suite
		    (session, EPOCH_NEXT,
		     session->internals.resumed_security_parameters.
		     cipher_suite);
		_gnutls_epoch_set_compression(session, EPOCH_NEXT,
					      session->internals.
					      resumed_security_parameters.
					      compression_method);

		session->internals.resumed = RESUME_TRUE;	/* we are resuming */

		return 0;
	} else {
		/* keep the new session id */
		session->internals.resumed = RESUME_FALSE;	/* we are not resuming */
		session->security_parameters.session_id_size =
		    session_id_len;
		if (session_id_len > 0) {
			memcpy(session->security_parameters.session_id, session_id,
			       session_id_len);
		}

		return -1;
	}
}