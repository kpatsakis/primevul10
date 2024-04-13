int _gnutls_handshake_get_session_hash(gnutls_session_t session, gnutls_datum_t *shash)
{
	const version_entry_st *ver = get_version(session);
	int ret;
	const mac_entry_st *me;
	uint8_t concat[2*MAX_HASH_SIZE];
	digest_hd_st td_md5;
	digest_hd_st td_sha;

	if (unlikely(ver == NULL))
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

	if (session->internals.handshake_hash_buffer_client_kx_len == 0 ||
	    (session->internals.handshake_hash_buffer.length <
	    session->internals.handshake_hash_buffer_client_kx_len)) {
			return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);
	}

	if (_gnutls_version_has_selectable_prf(ver)) { /* TLS 1.2+ */
		gnutls_mac_algorithm_t prf;

		prf = _gnutls_cipher_suite_get_prf(session->security_parameters.cipher_suite);
		if (prf == GNUTLS_MAC_UNKNOWN)
			return gnutls_assert_val(GNUTLS_E_UNKNOWN_PK_ALGORITHM);

		me = mac_to_entry(prf);

		ret =
		    _gnutls_hash_fast((gnutls_digest_algorithm_t)me->id,
				      session->internals.handshake_hash_buffer.
				      data,
				      session->internals.handshake_hash_buffer_client_kx_len,
				      concat);
		if (ret < 0)
			return gnutls_assert_val(ret);

		return _gnutls_set_datum(shash, concat, me->output_size);
	} else {
		ret = _gnutls_hash_init(&td_sha, hash_to_entry(GNUTLS_DIG_SHA1));
		if (ret < 0) {
			gnutls_assert();
			return ret;
		}

		_gnutls_hash(&td_sha,
			     session->internals.handshake_hash_buffer.data,
			     session->internals.handshake_hash_buffer_client_kx_len);

		_gnutls_hash_deinit(&td_sha, &concat[16]);

		ret =
		    _gnutls_hash_init(&td_md5,
				      hash_to_entry(GNUTLS_DIG_MD5));
		if (ret < 0)
			return gnutls_assert_val(ret);

		_gnutls_hash(&td_md5,
			     session->internals.handshake_hash_buffer.data,
			     session->internals.handshake_hash_buffer_client_kx_len);

		_gnutls_hash_deinit(&td_md5, concat);

		return _gnutls_set_datum(shash, concat, 36);
	}
}