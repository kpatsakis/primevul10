static ssize_t send_change_cipher_spec(gnutls_session_t session, int again)
{
	uint8_t *data;
	mbuffer_st *bufel;
	int ret;
	const version_entry_st *vers;

	if (again == 0) {
		bufel = _gnutls_handshake_alloc(session, 1);
		if (bufel == NULL)
			return gnutls_assert_val(GNUTLS_E_MEMORY_ERROR);

		vers = get_version(session);
		if (unlikely(vers == NULL))
			return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

		if (vers->id == GNUTLS_DTLS0_9)
			_mbuffer_set_uhead_size(bufel, 3);
		else
			_mbuffer_set_uhead_size(bufel, 1);
		_mbuffer_set_udata_size(bufel, 0);

		data = _mbuffer_get_uhead_ptr(bufel);

		data[0] = 1;
		if (vers->id == GNUTLS_DTLS0_9) {
			_gnutls_write_uint16(session->internals.dtls.
					     hsk_write_seq, &data[1]);
			session->internals.dtls.hsk_write_seq++;
		}

		ret =
		    _gnutls_handshake_io_cache_int(session,
						   GNUTLS_HANDSHAKE_CHANGE_CIPHER_SPEC,
						   bufel);
		if (ret < 0) {
			_mbuffer_xfree(&bufel);
			return gnutls_assert_val(ret);
		}

		_gnutls_handshake_log("REC[%p]: Sent ChangeCipherSpec\n",
				      session);
	}

	return 0;
}