_gnutls_ssl3_finished(gnutls_session_t session, int type, uint8_t * ret,
		      int sending)
{
	digest_hd_st td_md5;
	digest_hd_st td_sha;
	const char *mesg;
	int rc, len;

	if (sending)
		len = session->internals.handshake_hash_buffer.length;
	else
		len = session->internals.handshake_hash_buffer_prev_len;

	rc = _gnutls_hash_init(&td_sha, hash_to_entry(GNUTLS_DIG_SHA1));
	if (rc < 0)
		return gnutls_assert_val(rc);

	rc = _gnutls_hash_init(&td_md5, hash_to_entry(GNUTLS_DIG_MD5));
	if (rc < 0) {
		_gnutls_hash_deinit(&td_sha, NULL);
		return gnutls_assert_val(rc);
	}

	_gnutls_hash(&td_sha,
		     session->internals.handshake_hash_buffer.data, len);
	_gnutls_hash(&td_md5,
		     session->internals.handshake_hash_buffer.data, len);

	if (type == GNUTLS_SERVER)
		mesg = SSL3_SERVER_MSG;
	else
		mesg = SSL3_CLIENT_MSG;

	_gnutls_hash(&td_md5, mesg, SSL_MSG_LEN);
	_gnutls_hash(&td_sha, mesg, SSL_MSG_LEN);

	rc = _gnutls_mac_deinit_ssl3_handshake(&td_md5, ret,
					       session->security_parameters.
					       master_secret,
					       GNUTLS_MASTER_SIZE);
	if (rc < 0) {
		_gnutls_hash_deinit(&td_md5, NULL);
		_gnutls_hash_deinit(&td_sha, NULL);
		return gnutls_assert_val(rc);
	}

	rc = _gnutls_mac_deinit_ssl3_handshake(&td_sha, &ret[16],
					       session->security_parameters.
					       master_secret,
					       GNUTLS_MASTER_SIZE);
	if (rc < 0) {
		_gnutls_hash_deinit(&td_sha, NULL);
		return gnutls_assert_val(rc);
	}

	return 0;
}