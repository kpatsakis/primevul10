gnutls_handshake_set_timeout(gnutls_session_t session, unsigned int ms)
{
	if (ms == GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT)
		ms = DEFAULT_HANDSHAKE_TIMEOUT_MS;

	if (IS_DTLS(session)) {
		gnutls_dtls_set_timeouts(session, DTLS_RETRANS_TIMEOUT, ms);
		return;
	}

	session->internals.handshake_timeout_ms = ms;
}