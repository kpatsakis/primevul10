R_API int r_socket_flush(RSocket *s) {
#if HAVE_LIB_SSL
	if (s->is_ssl && s->bio) {
		return BIO_flush (s->bio);
	}
#endif
	return true;
}