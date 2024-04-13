static void build_radius_packet(AUTH_HDR *request, CONST char *user, CONST char *password, radius_conf_t *conf)
{
	char hostname[256];
	uint32_t ipaddr;

	hostname[0] = '\0';
	gethostname(hostname, sizeof(hostname) - 1);

	request->length = htons(AUTH_HDR_LEN);

	if (password) {		/* make a random authentication req vector */
		get_random_vector(request->vector);
	}

	add_attribute(request, PW_USER_NAME, (unsigned char *) user, strlen(user));

	/*
	 *	Add a password, if given.
	 */
	if (password) {
		add_password(request, PW_PASSWORD, password, conf->server->secret);

		/*
		 *	Add a NULL password to non-accounting requests.
		 */
	} else if (request->code != PW_ACCOUNTING_REQUEST) {
		add_password(request, PW_PASSWORD, "", conf->server->secret);
	}

	/* the packet is from localhost if on localhost, to make configs easier */
	if ((conf->server->ip.s_addr == ntohl(0x7f000001)) || (!hostname[0])) {
		ipaddr = 0x7f000001;
	} else {
		struct hostent *hp;

		if ((hp = gethostbyname(hostname)) == (struct hostent *) NULL) {
			ipaddr = 0x00000000;	/* no client IP address */
		} else {
			ipaddr = ntohl(*(uint32_t *) hp->h_addr); /* use the first one available */
		}
	}

	/* If we can't find an IP address, then don't add one */
	if (ipaddr) {
		add_int_attribute(request, PW_NAS_IP_ADDRESS, ipaddr);
	}

	/* There's always a NAS identifier */
	if (conf->client_id && *conf->client_id) {
		add_attribute(request, PW_NAS_IDENTIFIER, (unsigned char *) conf->client_id, strlen(conf->client_id));
	}

	/*
	 *	Add in the port (pid) and port type (virtual).
	 *
	 *	We might want to give the TTY name here, too.
	 */
	add_int_attribute(request, PW_NAS_PORT_ID, getpid());
	add_int_attribute(request, PW_NAS_PORT_TYPE, PW_NAS_PORT_TYPE_VIRTUAL);
}