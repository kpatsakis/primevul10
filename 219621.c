static int _gnutls_recv_finished(gnutls_session_t session)
{
	uint8_t data[MAX_VERIFY_DATA_SIZE], *vrfy;
	gnutls_buffer_st buf;
	int data_size;
	int ret;
	int vrfy_size;
	const version_entry_st *vers = get_version(session);

	if (unlikely(vers == NULL))
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

	ret =
	    _gnutls_recv_handshake(session, GNUTLS_HANDSHAKE_FINISHED,
				   0, &buf);
	if (ret < 0) {
		ERR("recv finished int", ret);
		gnutls_assert();
		return ret;
	}

	vrfy = buf.data;
	vrfy_size = buf.length;

	if (vers->id == GNUTLS_SSL3)
		data_size = 36;
	else
		data_size = 12;

	if (vrfy_size != data_size) {
		gnutls_assert();
		ret = GNUTLS_E_ERROR_IN_FINISHED_PACKET;
		goto cleanup;
	}

	if (vers->id == GNUTLS_SSL3) {
		ret =
		    _gnutls_ssl3_finished(session,
					  (session->security_parameters.
					   entity + 1) % 2, data, 0);
	} else {		/* TLS 1.0 */
		ret =
		    _gnutls_finished(session,
				     (session->security_parameters.entity +
				      1) % 2, data, 0);
	}

	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	if (memcmp(vrfy, data, data_size) != 0) {
		gnutls_assert();
		ret = GNUTLS_E_ERROR_IN_FINISHED_PACKET;
		goto cleanup;
	}

	ret = _gnutls_ext_sr_finished(session, data, data_size, 1);
	if (ret < 0) {
		gnutls_assert();
		goto cleanup;
	}

	if ((session->internals.resumed != RESUME_FALSE
	     && session->security_parameters.entity == GNUTLS_CLIENT)
	    || (session->internals.resumed == RESUME_FALSE
		&& session->security_parameters.entity == GNUTLS_SERVER)) {
		/* if we are a client resuming - or we are a server not resuming */
		_gnutls_handshake_log
		    ("HSK[%p]: recording tls-unique CB (recv)\n", session);
		memcpy(session->internals.cb_tls_unique, data, data_size);
		session->internals.cb_tls_unique_len = data_size;
	}


	session->internals.initial_negotiation_completed = 1;

      cleanup:
	_gnutls_buffer_clear(&buf);

	return ret;
}