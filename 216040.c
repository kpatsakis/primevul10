R_API RSocket *r_socket_new_from_fd(int fd) {
	RSocket *s = R_NEW0 (RSocket);
	if (s) {
		s->fd = fd;
	}
	return s;
}