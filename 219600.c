void _gnutls_handshake_hash_buffers_clear(gnutls_session_t session)
{
	session->internals.handshake_hash_buffer_prev_len = 0;
	session->internals.handshake_hash_buffer_client_kx_len = 0;
	_gnutls_buffer_clear(&session->internals.handshake_hash_buffer);
}