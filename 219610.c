int gnutls_handshake(gnutls_session_t session)
{
	int ret;
	record_parameters_st *params;

	/* sanity check. Verify that there are priorities setup.
	 */

	if (STATE == STATE0) {
		/* first call */
		if (session->internals.priorities.protocol.algorithms == 0)
			return gnutls_assert_val(GNUTLS_E_NO_PRIORITIES_WERE_SET);

		session->internals.handshake_in_progress = 1;
		gettime(&session->internals.handshake_start_time);
		if (session->internals.handshake_timeout_ms &&
		    session->internals.handshake_endtime == 0)
			    session->internals.handshake_endtime = session->internals.handshake_start_time.tv_sec +
			    	session->internals.handshake_timeout_ms / 1000;
	}

	ret =
	    _gnutls_epoch_get(session,
			      session->security_parameters.epoch_next,
			      &params);
	if (ret < 0) {
		/* We assume the epoch is not allocated if _gnutls_epoch_get fails. */
		ret =
		    _gnutls_epoch_alloc(session,
					session->security_parameters.
					epoch_next, NULL);
		if (ret < 0)
			return gnutls_assert_val(ret);
	}

	if (session->security_parameters.entity == GNUTLS_CLIENT) {
		do {
			ret = handshake_client(session);
		} while (ret == 1);
	} else {
		ret = handshake_server(session);
	}
	if (ret < 0) {
		/* In the case of a rehandshake abort
		 * we should reset the handshake's internal state.
		 */
		if (_gnutls_abort_handshake(session, ret) == 0)
			STATE = STATE0;

		return ret;
	}

	/* clear handshake buffer */
	_gnutls_handshake_hash_buffers_clear(session);

	if (IS_DTLS(session) == 0) {
		_gnutls_handshake_io_buffer_clear(session);
	} else {
		_dtls_async_timer_init(session);
	}

	_gnutls_handshake_internal_state_clear(session);

	session->security_parameters.epoch_next++;

	return 0;
}