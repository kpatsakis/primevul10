static int send_server_hello(gnutls_session_t session, int again)
{
	mbuffer_st *bufel = NULL;
	uint8_t *data = NULL;
	gnutls_buffer_st extdata;
	int pos = 0;
	int datalen, ret = 0;
	uint8_t comp;
	uint8_t session_id_len =
	    session->security_parameters.session_id_size;
	char buf[2 * GNUTLS_MAX_SESSION_ID_SIZE + 1];
	const version_entry_st *vers;

	_gnutls_buffer_init(&extdata);

	if (again == 0) {
		datalen = 2 + session_id_len + 1 + GNUTLS_RANDOM_SIZE + 3;
		ret =
		    _gnutls_gen_extensions(session, &extdata,
					   (session->internals.resumed ==
					    RESUME_TRUE) ?
					   GNUTLS_EXT_MANDATORY :
					   GNUTLS_EXT_ANY);
		if (ret < 0) {
			gnutls_assert();
			goto fail;
		}

		bufel =
		    _gnutls_handshake_alloc(session,
					    datalen + extdata.length);
		if (bufel == NULL) {
			gnutls_assert();
			ret = GNUTLS_E_MEMORY_ERROR;
			goto fail;
		}
		data = _mbuffer_get_udata_ptr(bufel);

		vers = get_version(session);
		if (unlikely(vers == NULL))
			return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

		data[pos++] = vers->major;
		data[pos++] = vers->minor;

		memcpy(&data[pos],
		       session->security_parameters.server_random,
		       GNUTLS_RANDOM_SIZE);
		pos += GNUTLS_RANDOM_SIZE;

		data[pos++] = session_id_len;
		if (session_id_len > 0) {
			memcpy(&data[pos],
			       session->security_parameters.session_id,
			       session_id_len);
		}
		pos += session_id_len;

		_gnutls_handshake_log("HSK[%p]: SessionID: %s\n", session,
				      _gnutls_bin2hex(session->
						      security_parameters.session_id,
						      session_id_len, buf,
						      sizeof(buf), NULL));

		memcpy(&data[pos],
		       session->security_parameters.cipher_suite, 2);
		pos += 2;

		comp =
		    _gnutls_compression_get_num(session->
						security_parameters.
						compression_method);
		data[pos++] = comp;

		if (extdata.length > 0) {
			memcpy(&data[pos], extdata.data, extdata.length);
		}
	}

	ret =
	    _gnutls_send_handshake(session, bufel,
				   GNUTLS_HANDSHAKE_SERVER_HELLO);

      fail:
	_gnutls_buffer_clear(&extdata);
	return ret;
}