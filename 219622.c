read_server_hello(gnutls_session_t session,
			  uint8_t * data, int datalen)
{
	uint8_t session_id_len = 0;
	int pos = 0;
	int ret = 0;
	gnutls_protocol_t version;
	int len = datalen;

	if (datalen < 38) {
		gnutls_assert();
		return GNUTLS_E_UNEXPECTED_PACKET_LENGTH;
	}

	_gnutls_handshake_log("HSK[%p]: Server's version: %d.%d\n",
			      session, data[pos], data[pos + 1]);

	DECR_LEN(len, 2);
	version = _gnutls_version_get(data[pos], data[pos + 1]);
	if (_gnutls_version_is_supported(session, version) == 0) {
		gnutls_assert();
		return GNUTLS_E_UNSUPPORTED_VERSION_PACKET;
	}

	if (_gnutls_set_current_version(session, version) < 0)
		return gnutls_assert_val(GNUTLS_E_UNSUPPORTED_VERSION_PACKET);

	pos += 2;

	DECR_LEN(len, GNUTLS_RANDOM_SIZE);
	ret = _gnutls_set_server_random(session, &data[pos]);
	if (ret < 0)
		return gnutls_assert_val(ret);

	pos += GNUTLS_RANDOM_SIZE;


	/* Read session ID
	 */
	DECR_LEN(len, 1);
	session_id_len = data[pos++];

	if (len < session_id_len || session_id_len > GNUTLS_MAX_SESSION_ID_SIZE) {
		gnutls_assert();
		return GNUTLS_E_UNSUPPORTED_VERSION_PACKET;
	}
	DECR_LEN(len, session_id_len);

	/* check if we are resuming and set the appropriate
	 * values;
	 */
	if (client_check_if_resuming
	    (session, &data[pos], session_id_len) == 0) {
		pos += session_id_len + 2 + 1;
		DECR_LEN(len, 2 + 1);

		ret =
		    _gnutls_parse_extensions(session, GNUTLS_EXT_MANDATORY,
					     &data[pos], len);
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}
		return 0;
	}

	pos += session_id_len;

	/* Check if the given cipher suite is supported and copy
	 * it to the session.
	 */

	DECR_LEN(len, 2);
	ret = set_client_ciphersuite(session, &data[pos]);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}
	pos += 2;

	/* move to compression 
	 */
	DECR_LEN(len, 1);

	ret = set_client_comp_method(session, data[pos++]);
	if (ret < 0) {
		gnutls_assert();
		return GNUTLS_E_UNKNOWN_COMPRESSION_ALGORITHM;
	}

	/* Parse extensions.
	 */
	ret =
	    _gnutls_parse_extensions(session, GNUTLS_EXT_ANY, &data[pos],
				     len);
	if (ret < 0) {
		gnutls_assert();
		return ret;
	}

	return ret;
}