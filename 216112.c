R_API bool r_socket_connect(RSocket *s, const char *host, const char *port, int proto, unsigned int timeout) {
	r_return_val_if_fail (s, false);
#if __WINDOWS__
#define gai_strerror gai_strerrorA
	struct sockaddr_in sa;
	WSADATA wsadata;

	if (WSAStartup (MAKEWORD (1, 1), &wsadata) == SOCKET_ERROR) {
		eprintf ("Error creating socket.");
		return false;
	}
#endif
	int ret;
	struct addrinfo hints = { 0 };
	struct addrinfo *res, *rp;
	if (!proto) {
		proto = R_SOCKET_PROTO_TCP;
	}
#if __UNIX__
	r_sys_signal (SIGPIPE, SIG_IGN);
#endif
	if (proto == R_SOCKET_PROTO_UNIX) {
#if __UNIX__
		if (!__connect_unix (s, host)) {
			return false;
		}
#endif
	} else {
		hints.ai_family = AF_UNSPEC; /* Allow IPv4 or IPv6 */
		hints.ai_protocol = proto;
		int gai = getaddrinfo (host, port, &hints, &res);
		if (gai != 0) {
			eprintf ("r_socket_connect: Error in getaddrinfo: %s (%s:%s)\n",
				gai_strerror (gai), host, port);
			return false;
		}
		for (rp = res; rp != NULL; rp = rp->ai_next) {
			int flag = 1;

			s->fd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (s->fd == -1) {
				perror ("socket");
				continue;
			}
			ret = setsockopt (s->fd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof (flag));
			if (ret < 0) {
				perror ("setsockopt");
				close (s->fd);
				s->fd = -1;
				continue;
			}

			r_socket_block_time (s, 0, 0, 0);
			ret = connect (s->fd, rp->ai_addr, rp->ai_addrlen);

			if (ret == 0) {
				freeaddrinfo (res);
				return true;
			}
			if (errno == EINPROGRESS) {
				struct timeval tv = {timeout, 0};
				fd_set wfds;
				FD_ZERO (&wfds);
				FD_SET (s->fd, &wfds);

				if ((ret = select (s->fd + 1, NULL, &wfds, NULL, &tv)) != -1) {
					if (r_socket_is_connected (s)) {
						freeaddrinfo (res);
						goto success;
					}
				} else {
					perror ("connect");
				}
			}
			r_socket_close (s);
		}
		freeaddrinfo (res);
		if (!rp) {
			eprintf ("Could not resolve address '%s' or failed to connect\n", host);
			return false;
		}
	}
success:
#if HAVE_LIB_SSL
	if (s->is_ssl) {
		s->ctx = SSL_CTX_new (SSLv23_client_method ());
		if (!s->ctx) {
			r_socket_close (s);
			return false;
		}
		s->sfd = SSL_new (s->ctx);
		SSL_set_fd (s->sfd, s->fd);
		int ret = SSL_connect (s->sfd);
		if (ret != 1) {
			int error = SSL_get_error (s->sfd, ret);
			int tries = 10;
			while (tries && ret && (error == SSL_ERROR_WANT_READ || error == SSL_ERROR_WANT_WRITE)) {
				struct timeval tv = {1, 0};
				fd_set rfds, wfds;
				FD_ZERO (&rfds);
				FD_ZERO (&wfds);
				if (error == SSL_ERROR_WANT_READ) {
					FD_SET (s->fd, &rfds);
				} else {
					FD_SET (s->fd, &wfds);
				}
				if ((ret = select (s->fd + 1, &rfds, &wfds, NULL, &tv)) < 1) {
					r_socket_close (s);
					return false;
				}
				ret = SSL_connect (s->sfd);
				if (ret == 1) {
					return true;
				}
				error = SSL_get_error (s->sfd, ret);
				tries--;
			}
			r_socket_close (s);
			return false;
		}
	}
#endif
	return true;
}