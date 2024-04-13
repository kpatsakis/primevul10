static int initialize(radius_conf_t *conf, int accounting)
{
	struct sockaddr salocal;
	char hostname[BUFFER_SIZE];
	char secret[BUFFER_SIZE];

	char buffer[BUFFER_SIZE];
	char *p;
	FILE *fserver;
	radius_server_t *server = NULL;
	struct sockaddr_in * s_in;
	int timeout;
	int line = 0;

	/* the first time around, read the configuration file */
	if ((fserver = fopen (conf_file, "r")) == (FILE*)NULL) {
		_pam_log(LOG_ERR, "Could not open configuration file %s: %s\n",
			conf_file, strerror(errno));
		return PAM_ABORT;
	}

	while (!feof(fserver) && (fgets (buffer, sizeof(buffer), fserver) != (char*) NULL) && (!ferror(fserver))) {
		line++;
		p = buffer;

		/*
		 *	Skip blank lines and whitespace
		 */
		while (*p && ((*p == ' ') || (*p == '\t') || (*p == '\r') || (*p == '\n'))) {
			p++;
		}

		/*
		 *	Nothing, or just a comment. Ignore the line.
		 */
		if ((!*p) || (*p == '#')) {
			continue;
		}

		timeout = 3;
		if (sscanf(p, "%s %s %d", hostname, secret, &timeout) < 2) {
			_pam_log(LOG_ERR, "ERROR reading %s, line %d: Could not read hostname or secret\n",
				 conf_file, line);
			continue;			/* invalid line */
		} else {				/* read it in and save the data */
			radius_server_t *tmp;

			tmp = malloc(sizeof(radius_server_t));
			if (server) {
				server->next = tmp;
				server = server->next;
			} else {
				conf->server = tmp;
				server= tmp;		/* first time */
			}

			/* sometime later do memory checks here */
			server->hostname = strdup(hostname);
			server->secret = strdup(secret);
			server->accounting = accounting;
			server->port = 0;

			if ((timeout < 1) || (timeout > 60)) {
				server->timeout = 3;
			} else {
				server->timeout = timeout;
			}
			server->next = NULL;
		}
	}
	fclose(fserver);

	if (!server) {		/* no server found, die a horrible death */
		_pam_log(LOG_ERR, "No RADIUS server found in configuration file %s\n",
			 conf_file);
		return PAM_AUTHINFO_UNAVAIL;
	}

	/* open a socket.	Dies if it fails */
	conf->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (conf->sockfd < 0) {
		_pam_log(LOG_ERR, "Failed to open RADIUS socket: %s\n", strerror(errno));
		return PAM_AUTHINFO_UNAVAIL;
	}

	/* set up the local end of the socket communications */
	s_in = (struct sockaddr_in *) &salocal;
	memset ((char *) s_in, '\0', sizeof(struct sockaddr));
	s_in->sin_family = AF_INET;
	s_in->sin_addr.s_addr = INADDR_ANY;
	s_in->sin_port = 0;
	

	if (bind(conf->sockfd, &salocal, sizeof (struct sockaddr_in)) < 0) {
		_pam_log(LOG_ERR, "Failed binding to port: %s", strerror(errno));
		close(conf->sockfd);
		return PAM_AUTHINFO_UNAVAIL;
	}

	return PAM_SUCCESS;
}