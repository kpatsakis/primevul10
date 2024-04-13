_gnutls_send_empty_handshake(gnutls_session_t session,
			     gnutls_handshake_description_t type,
			     int again)
{
	mbuffer_st *bufel;

	if (again == 0) {
		bufel = _gnutls_handshake_alloc(session, 0);
		if (bufel == NULL) {
			gnutls_assert();
			return GNUTLS_E_MEMORY_ERROR;
		}
	} else
		bufel = NULL;

	return _gnutls_send_handshake(session, bufel, type);
}