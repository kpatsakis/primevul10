_gnutls_finished(gnutls_session_t session, int type, void *ret,
		 int sending)
{
	const int siz = TLS_MSG_LEN;
	uint8_t concat[MAX_HASH_SIZE + 16 /*MD5 */ ];
	size_t hash_len;
	const char *mesg;
	int rc, len;

	if (sending)
		len = session->internals.handshake_hash_buffer.length;
	else
		len = session->internals.handshake_hash_buffer_prev_len;

	if (!_gnutls_version_has_selectable_prf(get_version(session))) {
		rc = _gnutls_hash_fast(GNUTLS_DIG_SHA1,
				       session->internals.
				       handshake_hash_buffer.data, len,
				       &concat[16]);
		if (rc < 0)
			return gnutls_assert_val(rc);

		rc = _gnutls_hash_fast(GNUTLS_DIG_MD5,
				       session->internals.
				       handshake_hash_buffer.data, len,
				       concat);
		if (rc < 0)
			return gnutls_assert_val(rc);

		hash_len = 20 + 16;
	} else {
		int algorithm =
		    _gnutls_cipher_suite_get_prf(session->
						 security_parameters.
						 cipher_suite);

		rc = _gnutls_hash_fast(algorithm,
				       session->internals.
				       handshake_hash_buffer.data, len,
				       concat);
		if (rc < 0)
			return gnutls_assert_val(rc);

		hash_len =
		    _gnutls_hash_get_algo_len(mac_to_entry(algorithm));
	}

	if (type == GNUTLS_SERVER) {
		mesg = SERVER_MSG;
	} else {
		mesg = CLIENT_MSG;
	}

	return _gnutls_PRF(session,
			   session->security_parameters.master_secret,
			   GNUTLS_MASTER_SIZE, mesg, siz, concat, hash_len,
			   12, ret);
}