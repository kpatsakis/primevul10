R_API ut8* r_socket_slurp(RSocket *s, int *len) {
	int blockSize = 4096;
	ut8 *ptr, *buf = malloc (blockSize);
	if (!buf) {
		return NULL;
	}
	int copied = 0;
	if (len) {
		*len = 0;
	}
	for (;;) {
		int rc = r_socket_read (s, buf + copied, blockSize);
		if (rc > 0) {
			copied += rc;
		}
		ptr = realloc (buf, copied + blockSize);
		if (!ptr) {
			break;
		}
		buf = ptr;
		if (rc < 1) {
			break;
		}
	}
	if (copied == 0) {
		R_FREE (buf);
	}
	if (len) {
		*len = copied;
	}
	return buf;
}