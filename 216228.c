R_API int r_socket_write(RSocket *s, void *buf, int len) {
	D { eprintf ("WRITE "); int i; ut8 *b = buf; for (i = 0; i<len; i++) { eprintf ("%02x ", b[i]); } eprintf ("\n"); }
	int ret, delta = 0;
#if __UNIX__
	r_sys_signal (SIGPIPE, SIG_IGN);
#endif
	for (;;) {
		int b = 1500; //65536; // Use MTU 1500?
		if (b > len) {
			b = len;
		}
#if HAVE_LIB_SSL
		if (s->is_ssl) {
			if (s->bio) {
				ret = BIO_write (s->bio, buf+delta, b);
			} else {
				ret = SSL_write (s->sfd, buf + delta, b);
			}
		} else
#endif
		{
			ret = send (s->fd, (char *)buf+delta, b, 0);
		}
		//if (ret == 0) return -1;
		if (ret < 1) {
			break;
		}
		if (ret == len) {
			return len;
		}
		delta += ret;
		len -= ret;
	}
	return (ret == -1)? -1 : delta;
}