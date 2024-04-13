R_API char *r_socket_to_string(RSocket *s) {
#if __WINDOWS__
	return r_str_newf ("fd%d", (int)(size_t)s->fd);
#elif __UNIX__
	char *str = NULL;
	struct sockaddr sa;
	socklen_t sl = sizeof (sa);
	memset (&sa, 0, sizeof (sa));
	if (!getpeername (s->fd, &sa, &sl)) {
		struct sockaddr_in *sain = (struct sockaddr_in*) &sa;
		ut8 *a = (ut8*) &(sain->sin_addr);
		if ((str = malloc (32))) {
			sprintf (str, "%d.%d.%d.%d:%d",
				a[0], a[1], a[2], a[3], ntohs (sain->sin_port));
		}
	} else {
		eprintf ("getperrname: failed\n"); //r_sys_perror ("getpeername");
	}
	return str;
#else
	return NULL;
#endif
}