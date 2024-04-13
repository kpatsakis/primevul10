R_API RSocket *r_socket_accept(RSocket *s) {
	RSocket *sock;
	socklen_t salen = sizeof (s->sa);
	if (!s) {
		return NULL;
	}
	sock = R_NEW0 (RSocket);
	if (!sock) {
		return NULL;
	}
	//signal (SIGPIPE, SIG_DFL);
	sock->fd = accept (s->fd, (struct sockaddr *)&s->sa, &salen);
	if (sock->fd == R_INVALID_SOCKET) {
		if (errno != EWOULDBLOCK) {
			// not just a timeout
			r_sys_perror ("accept");
		}
		free (sock);
		return NULL;
	}
#if HAVE_LIB_SSL
	sock->is_ssl = s->is_ssl;
	if (sock->is_ssl) {
		sock->sfd = NULL;
		sock->ctx = NULL;
		sock->bio = NULL;
		BIO *sbio = BIO_new_socket (sock->fd, BIO_NOCLOSE);
		sock->sfd = SSL_new (s->ctx);
		SSL_set_bio (sock->sfd, sbio, sbio);
		if (SSL_accept (sock->sfd) <= 0) {
			r_socket_free (sock);
			return NULL;
		}
		sock->bio = BIO_new (BIO_f_buffer ());
		sbio = BIO_new (BIO_f_ssl ());
		BIO_set_ssl (sbio, sock->sfd, BIO_CLOSE);
		BIO_push (sock->bio, sbio);
	}
#else
	sock->is_ssl = 0;
#endif
	return sock;
}