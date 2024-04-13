R_API bool r_socket_is_connected(RSocket *s) {
#if __WINDOWS__
	char buf[2];
	r_socket_block_time (s, 0, 0, 0);
#ifdef _MSC_VER
	int ret = recv (s->fd, (char*)&buf, 1, MSG_PEEK);
#else
	ssize_t ret = recv (s->fd, (char*)&buf, 1, MSG_PEEK);
#endif
	r_socket_block_time (s, 1, 0, 0);
	return ret == 1;
#else
	int error = 0;
	socklen_t len = sizeof (error);
	int ret = getsockopt (s->fd, SOL_SOCKET, SO_ERROR, &error, &len);
	if (ret != 0) {
		perror ("getsockopt");
		return false;
	}
	if (error != 0) {
		return false;
	}
	return true;
#endif
}