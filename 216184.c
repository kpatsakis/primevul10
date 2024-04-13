static size_t socket_slurp(RSocket *s, RBuffer *buf) {
	size_t i;
	if (r_socket_ready (s, 1, 0) != 1) {
		return 0;
	}
	r_socket_block_time (s, 1, 0, 1000);
	for (i = 0; i < SOCKET_HTTP_MAX_HEADER_LENGTH; i += 1) {
		ut8 c;
		int olen = r_socket_read_block (s, &c, 1);
		if (olen != 1) {
			r_buf_append_bytes (buf, (ut8 *)"", 1);
			break;
		}
		r_buf_append_bytes (buf, &c, 1);
	}
	return i;
}