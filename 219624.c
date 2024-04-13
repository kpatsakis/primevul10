static int _gnutls_send_finished(gnutls_session_t session, int again)
{
	mbuffer_st *bufel;
	uint8_t *data;
	int ret;
	size_t vdata_size = 0;
	const version_entry_st *vers;

	if (again == 0) {
		bufel =
		    _gnutls_handshake_alloc(session, 
					    MAX_VERIFY_DATA_SIZE);
		if (bufel == NULL) {
			gnutls_assert();
			return GNUTLS_E_MEMORY_ERROR;
		}
		data = _mbuffer_get_udata_ptr(bufel);

		vers = get_version(session);
		if (unlikely(vers == NULL))
			return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

		if (vers->id == GNUTLS_SSL3) {
			ret =
			    _gnutls_ssl3_finished(session,
						  session->
						  security_parameters.
						  entity, data, 1);
			_mbuffer_set_udata_size(bufel, 36);
		} else {	/* TLS 1.0+ */
			ret = _gnutls_finished(session,
					       session->
					       security_parameters.entity,
					       data, 1);
			_mbuffer_set_udata_size(bufel, 12);
		}

		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		vdata_size = _mbuffer_get_udata_size(bufel);

		ret =
		    _gnutls_ext_sr_finished(session, data, vdata_size, 0);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		if ((session->internals.resumed == RESUME_FALSE
		     && session->security_parameters.entity ==
		     GNUTLS_CLIENT)
		    || (session->internals.resumed != RESUME_FALSE
			&& session->security_parameters.entity ==
			GNUTLS_SERVER)) {
			/* if we are a client not resuming - or we are a server resuming */
			_gnutls_handshake_log
			    ("HSK[%p]: recording tls-unique CB (send)\n",
			     session);
			memcpy(session->internals.cb_tls_unique, data,
			       vdata_size);
			session->internals.cb_tls_unique_len = vdata_size;
		}

		ret =
		    _gnutls_send_handshake(session, bufel,
					   GNUTLS_HANDSHAKE_FINISHED);
	} else {
		ret =
		    _gnutls_send_handshake(session, NULL,
					   GNUTLS_HANDSHAKE_FINISHED);
	}

	return ret;
}