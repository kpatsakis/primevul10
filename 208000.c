connectINET(char *serverHost, uint16_t serverPort)
{
        struct sockaddr_in server;
        struct hostent *he;
        int asockd;
	struct sigaction action;
	action.sa_handler = connect_timeout;

        memset ((char *) &server, 0, sizeof (server));
        server.sin_addr.s_addr = inet_addr(serverHost);
        if ((asockd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
          ci_debug_printf(0, "ERROR connectINET: Can't create a socket.\n");
          return -1;
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(serverPort);

        if ((he = gethostbyname(serverHost)) == 0)
        {
          close(asockd);
          ci_debug_printf(0, "ERROR connectINET: Can't lookup hostname of %s\n", serverHost);
          return -1;
        }
        server.sin_addr = *(struct in_addr *) he->h_addr_list[0];
	sigaction(SIGALRM, &action, NULL);
	alarm(timeout);

        if (connect (asockd, (struct sockaddr *) &server, sizeof (struct sockaddr_in)) < 0) {
          close (asockd);
          ci_debug_printf(0, "ERROR connectINET: Can't connect on %s:%d.\n", serverHost,serverPort);
          return -1;
        }
	int err = errno;
	alarm(0);
	if (err == EINTR) {
          close(asockd);
	  ci_debug_printf(0, "ERROR connectINET: Timeout connecting to clamd on %s:%d.\n", serverHost,serverPort);
	}

        return asockd;
}