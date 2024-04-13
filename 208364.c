static int talk_radius(radius_conf_t *conf, AUTH_HDR *request, AUTH_HDR *response,
		       char *password, char *old_password, int tries)
{
	socklen_t salen;
	int total_length;
	fd_set set;
	struct timeval tv;
	time_t now, end;
	int rcode;
	struct sockaddr saremote;
	struct sockaddr_in *s_in = (struct sockaddr_in *) &saremote;
	radius_server_t *server = conf->server;
	int ok;
	int server_tries;
	int retval;

	/* ************************************************************ */
	/* Now that we're done building the request, we can send it */

	/*
	 Hmm... on password change requests, all of the found server information
	 could be saved with a pam_set_data(), which means even the radius_conf_t
	 information will have to be malloc'd at some point

	 On the other hand, we could just try all of the servers again in
	 sequence, on the off chance that one may have ended up fixing itself.

	 */

	/* loop over all available servers */
	while (server != NULL) {
		/* clear the response */
		memset(response, 0, sizeof(AUTH_HDR));

		/* only look up IP information as necessary */
		if ((retval = host2server(server)) != PAM_SUCCESS) {
			_pam_log(LOG_ERR,
				 "Failed looking up IP address for RADIUS server %s (errcode=%d)",
				 server->hostname, retval);
			ok = FALSE;
			goto next;		/* skip to the next server */
		}

		/* set up per-server IP && port configuration */
		memset ((char *) s_in, '\0', sizeof(struct sockaddr));
		s_in->sin_family = AF_INET;
		s_in->sin_addr.s_addr = htonl(server->ip.s_addr);
		s_in->sin_port = server->port;
		total_length = ntohs(request->length);

		if (!password) { 		/* make an RFC 2139 p6 request authenticator */
			get_accounting_vector(request, server);
		}

		server_tries = tries;
	send:
		/* send the packet */
		if (sendto(conf->sockfd, (char *) request, total_length, 0,
			   &saremote, sizeof(struct sockaddr_in)) < 0) {
			_pam_log(LOG_ERR, "Error sending RADIUS packet to server %s: %s",
				 server->hostname, strerror(errno));
			ok = FALSE;
			goto next;		/* skip to the next server */
		}

		/* ************************************************************ */
		/* Wait for the response, and verify it. */
		salen = sizeof(struct sockaddr);
		tv.tv_sec = server->timeout;	/* wait for the specified time */
		tv.tv_usec = 0;
		FD_ZERO(&set);			/* clear out the set */
		FD_SET(conf->sockfd, &set);	/* wait only for the RADIUS UDP socket */

		time(&now);
		end = now + tv.tv_sec;

		/* loop, waiting for the select to return data */
		ok = TRUE;
		while (ok) {
			rcode = select(conf->sockfd + 1, &set, NULL, NULL, &tv);

			/* select timed out */
			if (rcode == 0) {
				_pam_log(LOG_ERR, "RADIUS server %s failed to respond", server->hostname);
				if (--server_tries) {
					goto send;
				}
				ok = FALSE;
				break;			/* exit from the select loop */
			} else if (rcode < 0) {

				/* select had an error */
				if (errno == EINTR) {	/* we were interrupted */
					time(&now);

					if (now > end) {
						_pam_log(LOG_ERR, "RADIUS server %s failed to respond",
							 server->hostname);
						if (--server_tries) goto send;
						ok = FALSE;
						break;		/* exit from the select loop */
					}

					tv.tv_sec = end - now;
					if (tv.tv_sec == 0) {	/* keep waiting */
						tv.tv_sec = 1;
					}

				} else {			/* not an interrupt, it was a real error */
					_pam_log(LOG_ERR, "Error waiting for response from RADIUS server %s: %s",
						 server->hostname, strerror(errno));
					ok = FALSE;
					break;
				}

			/* the select returned OK */
			} else if (FD_ISSET(conf->sockfd, &set)) {

				/* try to receive some data */
				if ((total_length = recvfrom(conf->sockfd, (void *) response, BUFFER_SIZE,
						     	     0, &saremote, &salen)) < 0) {
					_pam_log(LOG_ERR, "error reading RADIUS packet from server %s: %s",
					 	 server->hostname, strerror(errno));
					ok = FALSE;
					break;

				/* there's data, see if it's valid */
				} else {
					char *p = server->secret;

					if ((ntohs(response->length) != total_length) ||
					    (ntohs(response->length) > BUFFER_SIZE)) {
						_pam_log(LOG_ERR, "RADIUS packet from server %s is corrupted",
						 	 server->hostname);
						ok = FALSE;
						break;
					}

					/* Check if we have the data OK. We should also check request->id */
					if (password) {
						if (old_password) {
#ifdef LIVINGSTON_PASSWORD_VERIFY_BUG_FIXED
							p = old_password;	/* what it should be */
#else
							p = "";			/* what it really is */
#endif
						}
					/*
					 * RFC 2139 p.6 says not do do this, but the Livingston 1.16
					 * server disagrees.	If the user says he wants the bug, give in.
					 */
					} else {		/* authentication request */
						if (conf->accounting_bug) {
							p = "";
						}
					}

					if (!verify_packet(p, response, request)) {
						_pam_log(LOG_ERR, "packet from RADIUS server %s failed verification: "
							 "The shared secret is probably incorrect.", server->hostname);
						ok = FALSE;
						break;
					}

					/*
					 * Check that the response ID matches the request ID.
					 */
					if (response->id != request->id) {
						_pam_log(LOG_WARNING, "Response packet ID %d does not match the "
							 "request packet ID %d: verification of packet fails",
							 response->id, request->id);
						ok = FALSE;
						break;
					}
				}

				/*
				 * Whew! The select is done. It hasn't timed out, or errored out.
				 * It's our descriptor.	We've got some data. It's the right size.
				 * The packet is valid.
				 * NOW, we can skip out of the select loop, and process the packet
				 */
				break;
			}
			/* otherwise, we've got data on another descriptor, keep select'ing */
		}

			/* go to the next server if this one didn't respond */
		next:
		if (!ok) {
			radius_server_t *old;	/* forget about this server */

			old = server;
			server = server->next;
			conf->server = server;

			_pam_forget(old->secret);
			free(old->hostname);
			free(old);

			if (server) {		/* if there's more servers to check */
				/* get a new authentication vector, and update the passwords */
				get_random_vector(request->vector);
				request->id = request->vector[0];

				/* update passwords, as appropriate */
				if (password) {
					get_random_vector(request->vector);
					if (old_password) {	/* password change request */
						add_password(request, PW_PASSWORD, password, old_password);
						add_password(request, PW_OLD_PASSWORD, old_password, old_password);
					} else {		/* authentication request */
						add_password(request, PW_PASSWORD, password, server->secret);
					}
				}
			}
			continue;

		} else {
			/* we've found one that does respond, forget about the other servers */
			cleanup(server->next);
			server->next = NULL;
			live_server = server;	/* we've got a live one! */
			break;
		}
	}

	if (!server) {
		_pam_log(LOG_ERR, "All RADIUS servers failed to respond.");
		if (conf->localifdown)
			retval = PAM_IGNORE;
		else
			retval = PAM_AUTHINFO_UNAVAIL;
	} else {
		retval = PAM_SUCCESS;
	}

	return retval;
}