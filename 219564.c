read_client_hello(gnutls_session_t session, uint8_t * data,
			  int datalen)
{
	uint8_t session_id_len;
	int pos = 0, ret;
	uint16_t suite_size, comp_size;
	gnutls_protocol_t adv_version;
	int neg_version, sret = 0;
	int len = datalen;
	uint8_t *suite_ptr, *comp_ptr, *session_id;

	DECR_LEN(len, 2);

	_gnutls_handshake_log("HSK[%p]: Client's version: %d.%d\n",
			      session, data[pos], data[pos + 1]);

	adv_version = _gnutls_version_get(data[pos], data[pos + 1]);

	set_adv_version(session, data[pos], data[pos + 1]);
	pos += 2;

	neg_version = _gnutls_negotiate_version(session, adv_version);
	if (neg_version < 0) {
		gnutls_assert();
		return neg_version;
	}

	_gnutls_handshake_log("HSK[%p]: Selected version %s\n",
		     session, gnutls_protocol_get_name(neg_version));

	/* Read client random value.
	 */
	DECR_LEN(len, GNUTLS_RANDOM_SIZE);
	ret = _gnutls_set_client_random(session, &data[pos]);
	if (ret < 0)
		return gnutls_assert_val(ret);

	pos += GNUTLS_RANDOM_SIZE;

	ret = _gnutls_set_server_random(session, NULL);
	if (ret < 0)
		return gnutls_assert_val(ret);

	session->security_parameters.timestamp = gnutls_time(NULL);

	DECR_LEN(len, 1);
	session_id_len = data[pos++];

	/* RESUME SESSION 
	 */
	if (session_id_len > GNUTLS_MAX_SESSION_ID_SIZE) {
		gnutls_assert();
		return GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
	}
	DECR_LEN(len, session_id_len);
	session_id = &data[pos];
	pos += session_id_len;

	if (IS_DTLS(session)) {
		int cookie_size;

		DECR_LEN(len, 1);
		cookie_size = data[pos++];
		DECR_LEN(len, cookie_size);
		pos += cookie_size;
	}

	ret =
	    _gnutls_server_restore_session(session, session_id,
					   session_id_len);

	if (session_id_len > 0)
		session->internals.resumption_requested = 1;

	if (ret == 0) {		/* resumed using default TLS resumption! */
		/* Parse only the safe renegotiation extension
		 * We don't want to parse any other extensions since
		 * we don't want new extension values to overwrite the
		 * resumed ones.
		 */

		/* move forward to extensions */
		DECR_LEN(len, 2);
		suite_size = _gnutls_read_uint16(&data[pos]);
		pos += 2;

		DECR_LEN(len, suite_size);
		pos += suite_size;

		DECR_LEN(len, 1);
		comp_size = data[pos++];	/* z is the number of compression methods */
		DECR_LEN(len, comp_size);
		pos += comp_size;

		ret =
		    _gnutls_parse_extensions(session, GNUTLS_EXT_MANDATORY,
					     &data[pos], len);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		ret = resume_copy_required_values(session);
		if (ret < 0)
			return gnutls_assert_val(ret);

		session->internals.resumed = RESUME_TRUE;

		return _gnutls_user_hello_func(session, adv_version);
	} else {
		_gnutls_generate_session_id(session->security_parameters.
					    session_id,
					    &session->security_parameters.
					    session_id_size);

		session->internals.resumed = RESUME_FALSE;
	}

	/* Remember ciphersuites for later
	 */
	DECR_LEN(len, 2);
	suite_size = _gnutls_read_uint16(&data[pos]);
	pos += 2;

	DECR_LEN(len, suite_size);
	suite_ptr = &data[pos];
	pos += suite_size;

	/* Point to the compression methods
	 */
	DECR_LEN(len, 1);
	comp_size = data[pos++];	/* z is the number of compression methods */

	DECR_LEN(len, comp_size);
	comp_ptr = &data[pos];
	pos += comp_size;

	/* Parse the extensions (if any)
	 *
	 * Unconditionally try to parse extensions; safe renegotiation uses them in
	 * sslv3 and higher, even though sslv3 doesn't officially support them.
	 */
	ret = _gnutls_parse_extensions(session, GNUTLS_EXT_APPLICATION,
				       &data[pos], len);
	/* len is the rest of the parsed length */
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	/* we cache this error code */
	sret = _gnutls_user_hello_func(session, adv_version);
	if (sret < 0 && sret != GNUTLS_E_INT_RET_0) {
		gnutls_assert();
		return sret;
	}

	ret = _gnutls_parse_extensions(session, GNUTLS_EXT_MANDATORY,
				       &data[pos], len);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	ret =
	    _gnutls_parse_extensions(session, GNUTLS_EXT_TLS, &data[pos],
				     len);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	/* resumed by session_ticket extension */
	if (session->internals.resumed != RESUME_FALSE) {
		/* to indicate the client that the current session is resumed */
		memcpy(session->internals.resumed_security_parameters.
		       session_id, session_id, session_id_len);
		session->internals.resumed_security_parameters.
		    session_id_size = session_id_len;

		session->internals.resumed_security_parameters.
		    max_record_recv_size =
		    session->security_parameters.max_record_recv_size;
		session->internals.resumed_security_parameters.
		    max_record_send_size =
		    session->security_parameters.max_record_send_size;

		ret = resume_copy_required_values(session);
		if (ret < 0)
			return gnutls_assert_val(ret);

		return 0;
	}

	/* select an appropriate cipher suite
	 */
	ret = _gnutls_server_select_suite(session, suite_ptr, suite_size);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	/* select appropriate compression method */
	ret =
	    server_select_comp_method(session, comp_ptr,
					      comp_size);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	return sret;
}