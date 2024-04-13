R_API int r_socket_read_block(RSocket *s, ut8 *buf, int len) {
	int ret = 0;
	for (ret = 0; ret < len; ) {
		int r = r_socket_read (s, buf + ret, len - ret);
		if (r == -1) {
#if HAVE_LIB_SSL
			if (SSL_get_error (s->sfd, r) == SSL_ERROR_WANT_READ) {
				if (r_socket_ready (s, 1, 0) == 1) {
					continue;
				}
			}
#endif
			return -1;
		}
		if (r < 1) {
			break;
		}
		ret += r;
	}
	return ret;
}