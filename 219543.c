handshake_hash_add_sent(gnutls_session_t session,
				gnutls_handshake_description_t type,
				uint8_t * dataptr, uint32_t datalen)
{
	int ret;
	const version_entry_st *vers = get_version(session);

	if (unlikely(vers == NULL))
		return gnutls_assert_val(GNUTLS_E_INTERNAL_ERROR);

	/* We don't check for GNUTLS_HANDSHAKE_HELLO_VERIFY_REQUEST because it
	 * is not sent via that channel.
	 */
	if (type != GNUTLS_HANDSHAKE_HELLO_REQUEST) {
		CHECK_SIZE(datalen);

		if (vers->id == GNUTLS_DTLS0_9) {
			/* Old DTLS doesn't include the header in the MAC */
			if (datalen < 12) {
				gnutls_assert();
				return GNUTLS_E_INTERNAL_ERROR;
			}
			dataptr += 12;
			datalen -= 12;

			if (datalen == 0)
				return 0;
		}

		ret =
		    _gnutls_buffer_append_data(&session->internals.
					       handshake_hash_buffer,
					       dataptr, datalen);
		if (ret < 0)
			return gnutls_assert_val(ret);

		if (type == GNUTLS_HANDSHAKE_CLIENT_KEY_EXCHANGE)
			session->internals.handshake_hash_buffer_client_kx_len =
				session->internals.handshake_hash_buffer.length;

		return 0;
	}

	return 0;
}