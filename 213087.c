static void rap_break (void *u) {
	RIORap *rior = (RIORap*) u;
	if (u) {
		r_socket_close (rior->fd);
		rior->fd = NULL;
	}
}