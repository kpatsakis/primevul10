R_API RSocket *r_socket_new(bool is_ssl) {
	RSocket *s = R_NEW0 (RSocket);
	if (!s) {
		return NULL;
	}
	s->is_ssl = is_ssl;
	s->port = 0;
#if __UNIX_
	r_sys_signal (SIGPIPE, SIG_IGN);
#endif
	s->local = 0;
	s->fd = R_INVALID_SOCKET;
#if HAVE_LIB_SSL
	if (is_ssl) {
		s->sfd = NULL;
		s->ctx = NULL;
		s->bio = NULL;
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
		if (!SSL_library_init ()) {
			r_socket_free (s);
			return NULL;
		}
		SSL_load_error_strings ();
#endif
	}
#endif
	return s;
}