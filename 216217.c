R_API int r_socket_gets(RSocket *s, char *buf,	int size) {
	int i = 0;
	int ret = 0;

	if (s->fd == R_INVALID_SOCKET) {
		return -1;
	}
	while (i < size) {
		ret = r_socket_read (s, (ut8 *)buf + i, 1);
		if (ret == 0) {
			if (i > 0) {
				return i;
			}
			return -1;
		}
		if (ret < 0) {
			r_socket_close (s);
			return i == 0? -1: i;
		}
		if (buf[i] == '\r' || buf[i] == '\n') {
			buf[i] = 0;
			break;
		}
		i += ret;
	}
	buf[i] = '\0';
	return i;
}