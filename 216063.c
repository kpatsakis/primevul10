R_API int r_socket_free(RSocket *s) {
	int res = r_socket_close (s);
#if HAVE_LIB_SSL
	if (s && s->is_ssl) {
		if (s->sfd) {
			SSL_free (s->sfd);
		}
		if (s->ctx) {
			SSL_CTX_free (s->ctx);
		}
	}
#endif
	free (s);
	return res;
}