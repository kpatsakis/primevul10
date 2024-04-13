R_API int r_socket_close(RSocket *s) {
	int ret = false;
	if (!s) {
		return false;
	}
	if (s->fd != R_INVALID_SOCKET) {
#if __UNIX__
		shutdown (s->fd, SHUT_RDWR);
#endif
#if __WINDOWS__
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms740481(v=vs.85).aspx
		shutdown (s->fd, SD_SEND);
		if (r_socket_ready (s, 0, 250)) {
			do {
				char buf = 0;
				ret = recv (s->fd, &buf, 1, 0);
			} while (ret != 0 && ret != SOCKET_ERROR);
		}
		ret = closesocket (s->fd);
#else
		ret = close (s->fd);
#endif
		s->fd = R_INVALID_SOCKET;
	}
#if HAVE_LIB_SSL
	if (s->is_ssl && s->sfd) {
		SSL_free (s->sfd);
		s->sfd = NULL;
	}
#endif
	return ret;
}