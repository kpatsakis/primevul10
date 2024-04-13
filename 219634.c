handshake_hash_add_recvd(gnutls_session_t session,
				 gnutls_handshake_description_t recv_type,
				 uint8_t * header, uint16_t header_size,
				 uint8_t * dataptr, uint32_t datalen)
{
	int ret;
	const version_entry_st *vers = get_version(session);

	if (unlikely(vers == NULL))
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

	if ((vers->id != GNUTLS_DTLS0_9 &&
	     recv_type == GNUTLS_HANDSHAKE_HELLO_VERIFY_REQUEST) ||
	    recv_type == GNUTLS_HANDSHAKE_HELLO_REQUEST)
		return 0;

	CHECK_SIZE(header_size + datalen);

	session->internals.handshake_hash_buffer_prev_len =
	    session->internals.handshake_hash_buffer.length;

	if (vers->id != GNUTLS_DTLS0_9) {
		ret =
		    _gnutls_buffer_append_data(&session->internals.
					       handshake_hash_buffer,
					       header, header_size);
		if (ret < 0)
			return gnutls_assert_val(ret);
	}
	if (datalen > 0) {
		ret =
		    _gnutls_buffer_append_data(&session->internals.
					       handshake_hash_buffer,
					       dataptr, datalen);
		if (ret < 0)
			return gnutls_assert_val(ret);
	}

	/* save the size until client KX. That is because the TLS
	 * session hash is calculated up to this message.
	 */
	if (recv_type == GNUTLS_HANDSHAKE_CLIENT_KEY_EXCHANGE)
		session->internals.handshake_hash_buffer_client_kx_len =
			session->internals.handshake_hash_buffer.length;

	return 0;
}