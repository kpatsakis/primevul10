    static int call_hook_func(gnutls_session_t session,
			      gnutls_handshake_description_t type,
			      int post, unsigned incoming,
			      const uint8_t *data, unsigned data_size)
{
	gnutls_datum_t msg = {(void*)data, data_size};

	if (session->internals.h_hook != NULL) {
		if ((session->internals.h_type == type
		     || session->internals.h_type == GNUTLS_HANDSHAKE_ANY)
		    && (session->internals.h_post == post
			|| session->internals.h_post == GNUTLS_HOOK_BOTH))
			return session->internals.h_hook(session, type,
							 post, incoming, &msg);
	}
	return 0;
}