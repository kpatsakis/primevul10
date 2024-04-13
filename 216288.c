static bool __connect_unix(RSocket *s, const char *file) {
	struct sockaddr_un addr;
	int sock = socket (PF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		free (s);
		return false;
	}
	// TODO: set socket options
	addr.sun_family = AF_UNIX;
	strncpy (addr.sun_path, file, sizeof (addr.sun_path)-1);

	if (connect (sock, (struct sockaddr *)&addr, sizeof(addr))==-1) {
		close (sock);
		free (s);
		return false;
	}
	s->fd = sock;
	s->is_ssl = false;
	return true;
}