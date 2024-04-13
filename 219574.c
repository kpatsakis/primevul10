handshake_hash_buffer_empty(gnutls_session_t session)
{

	_gnutls_buffers_log("BUF[HSK]: Emptied buffer\n");

	session->internals.handshake_hash_buffer_prev_len = 0;
	session->internals.handshake_hash_buffer.length = 0;
	return;
}