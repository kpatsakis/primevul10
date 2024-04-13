static int host2server(radius_server_t *server)
{
	char *p;

	if ((p = strchr(server->hostname, ':')) != NULL) {
		*(p++) = '\0';		/* split the port off from the host name */
	}

	if ((server->ip.s_addr = get_ipaddr(server->hostname)) == ((uint32_t)0)) {
		DPRINT(LOG_DEBUG, "DEBUG: get_ipaddr(%s) returned 0.\n", server->hostname);
		return PAM_AUTHINFO_UNAVAIL;
	}

	/*
	 *	If the server port hasn't already been defined, go get it.
	 */
	if (!server->port) {
		if (p && isdigit(*p)) {	/* the port looks like it's a number */
			unsigned int i = atoi(p) & 0xffff;

			if (!server->accounting) {
				server->port = htons((uint16_t) i);
			} else {
				server->port = htons((uint16_t) (i + 1));
			}
		} else {			/* the port looks like it's a name */
			struct servent *svp;

			if (p) {			/* maybe it's not "radius" */
				svp = getservbyname (p, "udp");
				/* quotes allow distinction from above, lest p be radius or radacct */
				DPRINT(LOG_DEBUG, "DEBUG: getservbyname('%s', udp) returned %p.\n", p, svp);
				*(--p) = ':';		/* be sure to put the delimiter back */
			} else {
				if (!server->accounting) {
					svp = getservbyname ("radius", "udp");
					DPRINT(LOG_DEBUG, "DEBUG: getservbyname(radius, udp) returned %p.\n", svp);
				} else {
					svp = getservbyname ("radacct", "udp");
					DPRINT(LOG_DEBUG, "DEBUG: getservbyname(radacct, udp) returned %p.\n", svp);
				}
			}

			if (svp == (struct servent *) 0) {
				/* debugging above... */
				return PAM_AUTHINFO_UNAVAIL;
			}

			server->port = svp->s_port;
		}
	}

	return PAM_SUCCESS;
}