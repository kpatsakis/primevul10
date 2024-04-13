R_API int r_socket_ready(RSocket *s, int secs, int usecs) {
	fd_set rfds;
	struct timeval tv = {secs, usecs};
	if (s->fd == R_INVALID_SOCKET) {
		return -1;
	}
	FD_ZERO (&rfds);
	FD_SET (s->fd, &rfds);
	return select (s->fd + 1, &rfds, NULL, NULL, &tv);
}