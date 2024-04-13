R_API int r_socket_close_fd(RSocket *s) {
#ifdef _MSC_VER
	return s->fd != INVALID_SOCKET ? closesocket (s->fd) : false;
#else
	return s->fd != -1 ? close (s->fd) : false;
#endif
}