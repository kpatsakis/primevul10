R_API bool r_socket_listen(RSocket *s, const char *port, const char *certfile) {
	int optval = 1;
	int ret;
	struct linger linger = { 0 };

	if (s->proto == R_SOCKET_PROTO_UNIX) {
#if __UNIX__
		return __listen_unix (s, port);
#endif
		return false;
	}

	if (r_sandbox_enable (0)) {
		return false;
	}
#if __WINDOWS__
	WSADATA wsadata;
	if (WSAStartup (MAKEWORD (1, 1), &wsadata) == SOCKET_ERROR) {
		eprintf ("Error creating socket.");
		return false;
	}
#endif
	if ((s->fd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) == R_INVALID_SOCKET) {
		return false;
	}

	linger.l_onoff = 1;
	linger.l_linger = 1;
	ret = setsockopt (s->fd, SOL_SOCKET, SO_LINGER, (void*)&linger, sizeof (linger));
	if (ret < 0) {
		return false;
	}
	{ // fix close after write bug //
	int x = 1500; // FORCE MTU
	ret = setsockopt (s->fd, SOL_SOCKET, SO_SNDBUF, (void*)&x, sizeof (int));
	if (ret < 0) {
		return false;
	}
	}
	ret = setsockopt (s->fd, SOL_SOCKET, SO_REUSEADDR, (void*)&optval, sizeof optval);
	if (ret < 0) {
		return false;
	}

	memset (&s->sa, 0, sizeof (s->sa));
	s->sa.sin_family = AF_INET;
	s->sa.sin_addr.s_addr = htonl (s->local? INADDR_LOOPBACK: INADDR_ANY);
	s->port = r_socket_port_by_name (port);
	if (s->port < 1) {
		return false;
	}
	s->sa.sin_port = htons (s->port); // TODO honor etc/services
	if (bind (s->fd, (struct sockaddr *)&s->sa, sizeof (s->sa)) < 0) {
		r_sys_perror ("bind");
#ifdef _MSC_VER
		closesocket (s->fd);
#else
		close (s->fd);
#endif
		return false;
	}
#if __UNIX__
	r_sys_signal (SIGPIPE, SIG_IGN);
#endif
	if (listen (s->fd, 32) < 0) {
#ifdef _MSC_VER
		closesocket (s->fd);
#else
		close (s->fd);
#endif
		return false;
	}
#if HAVE_LIB_SSL
	if (s->is_ssl) {
		s->ctx = SSL_CTX_new (SSLv23_method ());
		if (!s->ctx) {
			r_socket_free (s);
			return false;
		}
		if (!SSL_CTX_use_certificate_chain_file (s->ctx, certfile)) {
			r_socket_free (s);
			return false;
		}
		if (!SSL_CTX_use_PrivateKey_file (s->ctx, certfile, SSL_FILETYPE_PEM)) {
			r_socket_free (s);
			return false;
		}
		SSL_CTX_set_verify_depth (s->ctx, 1);
	}
#endif
	return true;
}