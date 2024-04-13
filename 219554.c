static int send_client_hello(gnutls_session_t session, int again)
{
	mbuffer_st *bufel = NULL;
	int type;
	int ret = 0;
	const version_entry_st *hver;
	uint8_t tver[2];
	gnutls_buffer_st extdata;
	int rehandshake = 0;
	uint8_t session_id_len =
	    session->internals.resumed_security_parameters.session_id_size;

	_gnutls_buffer_init(&extdata);

	/* note that rehandshake is different than resuming
	 */
	if (session->security_parameters.session_id_size)
		rehandshake = 1;

	if (again == 0) {
		/* if we are resuming a session then we set the
		 * version number to the previously established.
		 */
		if (session->internals.resumption_requested == 0 &&
		    session->internals.premaster_set == 0) {
			if (rehandshake)	/* already negotiated version thus version_max == negotiated version */
				hver = get_version(session);
			else	/* new handshake. just get the max */
				hver =
				    version_to_entry(_gnutls_version_max
						     (session));
		} else {
			/* we are resuming a session */
			hver =
			    session->internals.resumed_security_parameters.
			    pversion;
		}

		if (hver == NULL) {
			gnutls_assert();
			return GNUTLS_E_INTERNAL_ERROR;
		}

		if (unlikely(session->internals.default_hello_version[0] != 0)) {
			tver[0] = session->internals.default_hello_version[0];
			tver[1] = session->internals.default_hello_version[1];
		} else {
			tver[0] = hver->major;
			tver[1] = hver->minor;
		}
		ret = _gnutls_buffer_append_data(&extdata, tver, 2);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		/* Set the version we advertized as maximum 
		 * (RSA uses it).
		 */
		set_adv_version(session, hver->major, hver->minor);
		if (_gnutls_set_current_version(session, hver->id) < 0)
			return gnutls_assert_val(GNUTLS_E_UNSUPPORTED_VERSION_PACKET);

		if (session->internals.priorities.min_record_version != 0) {
			/* Advertize the SSL 3.0 record packet version in
			 * record packets during the handshake.
			 * That is to avoid confusing implementations
			 * that do not support TLS 1.2 and don't know
			 * how 3,3 version of record packets look like.
			 */
			const version_entry_st *v = _gnutls_version_lowest(session);

			if (v == NULL) {
				gnutls_assert();
				return GNUTLS_E_INTERNAL_ERROR;
			} else {
				_gnutls_record_set_default_version(session,
								   v->major, v->minor);
			}
		}

		/* In order to know when this session was initiated.
		 */
		session->security_parameters.timestamp = gnutls_time(NULL);

		/* Generate random data 
		 */
		if (!IS_DTLS(session)
		    || session->internals.dtls.hsk_hello_verify_requests ==
		    0) {
			ret = _gnutls_set_client_random(session, NULL);
			if (ret < 0)
				return gnutls_assert_val(ret);

		}

		ret = _gnutls_buffer_append_data(&extdata,
					session->security_parameters.client_random,
					GNUTLS_RANDOM_SIZE);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		/* Copy the Session ID 
		 */
		ret = _gnutls_buffer_append_data_prefix(&extdata, 8, 
						session->internals.resumed_security_parameters.session_id,
						session_id_len);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		/* Copy the DTLS cookie
		 */
		if (IS_DTLS(session)) {
			ret = _gnutls_buffer_append_data_prefix(&extdata, 8, session->internals.dtls.cookie,
				session->internals.dtls.cookie_len);
			if (ret < 0) {
				gnutls_assert();
				goto cleanup;
			}
		}

		/* Copy the ciphersuites.
		 *
		 * If using SSLv3 Send TLS_RENEGO_PROTECTION_REQUEST SCSV for MITM
		 * prevention on initial negotiation (but not renegotiation; that's
		 * handled with the RI extension below).
		 */
		if (!session->internals.initial_negotiation_completed &&
		    session->security_parameters.entity == GNUTLS_CLIENT &&
		    (hver->id == GNUTLS_SSL3 &&
		     session->internals.priorities.no_extensions != 0)) {
			ret =
			    copy_ciphersuites(session, &extdata,
					      TRUE);
			if (session->security_parameters.entity == GNUTLS_CLIENT)
				_gnutls_extension_list_add(session,
						   GNUTLS_EXTENSION_SAFE_RENEGOTIATION);
		} else
			ret =
			    copy_ciphersuites(session, &extdata,
					      FALSE);

		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		/* Copy the compression methods.
		 */
		ret = copy_comp_methods(session, &extdata);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}

		/* Generate and copy TLS extensions.
		 */
		if (session->internals.priorities.no_extensions == 0) {
			if (_gnutls_version_has_extensions(hver)) {
				type = GNUTLS_EXT_ANY;
			} else {
				type = GNUTLS_EXT_MANDATORY;
			}

			ret =
			    _gnutls_gen_extensions(session, &extdata,
						   type);
			if (ret < 0) {
				gnutls_assert();
				goto cleanup;
			}
		}

		bufel =
		    _gnutls_handshake_alloc(session, extdata.length);
		if (bufel == NULL) {
			gnutls_assert();
			ret = GNUTLS_E_MEMORY_ERROR;
			goto cleanup; 
		}
		_mbuffer_set_udata_size(bufel, 0);

		ret =
		    _mbuffer_append_data(bufel, extdata.data,
					 extdata.length);
		if (ret < 0) {
			gnutls_assert();
			goto cleanup;
		}
	}

	_gnutls_buffer_clear(&extdata);

	return
	    _gnutls_send_handshake(session, bufel,
				   GNUTLS_HANDSHAKE_CLIENT_HELLO);

      cleanup:
	_mbuffer_xfree(&bufel);
	_gnutls_buffer_clear(&extdata);
	return ret;
}