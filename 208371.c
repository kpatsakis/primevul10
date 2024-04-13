PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh,int flags,int argc,CONST char **argv)
{
	CONST char *user;
	CONST char *userinfo;
	char *password = NULL;
	CONST char *rhost;
	char *resp2challenge = NULL;
	int ctrl;
	int retval = PAM_AUTH_ERR;
	int num_challenge = 0;

	char recv_buffer[4096];
	char send_buffer[4096];
	AUTH_HDR *request = (AUTH_HDR *) send_buffer;
	AUTH_HDR *response = (AUTH_HDR *) recv_buffer;
	radius_conf_t config;

	ctrl = _pam_parse(argc, argv, &config);

	/* grab the user name */
	retval = pam_get_user(pamh, &user, NULL);
	PAM_FAIL_CHECK;

	/* check that they've entered something, and not too long, either */
	if ((user == NULL) || (strlen(user) > MAXPWNAM)) {
		int *pret = malloc(sizeof(int));
		*pret = PAM_USER_UNKNOWN;
		pam_set_data(pamh, "rad_setcred_return", (void *) pret, _int_free);

		DPRINT(LOG_DEBUG, "User name was NULL, or too long");
		return PAM_USER_UNKNOWN;
	}
	DPRINT(LOG_DEBUG, "Got user name %s", user);

	if (ctrl & PAM_RUSER_ARG) {
		retval = pam_get_item(pamh, PAM_RUSER, (CONST void **) &userinfo);
		PAM_FAIL_CHECK;
		DPRINT(LOG_DEBUG, "Got PAM_RUSER name %s", userinfo);

		if (!strcmp("root", user)) {
			user = userinfo;
			DPRINT(LOG_DEBUG, "Username now %s from ruser", user);
		} else {
			DPRINT(LOG_DEBUG, "Skipping ruser for non-root auth");
		}
	}

	/*
	 * Get the IP address of the authentication server
	 * Then, open a socket, and bind it to a port
	 */
	retval = initialize(&config, FALSE);
	PAM_FAIL_CHECK;

	/*
	 * If there's no client id specified, use the service type, to help
	 * keep track of which service is doing the authentication.
	 */
	if (!config.client_id) {
		retval = pam_get_item(pamh, PAM_SERVICE, (CONST void **) &config.client_id);
		PAM_FAIL_CHECK;
	}

	/* now we've got a socket open, so we've got to clean it up on error */
#undef PAM_FAIL_CHECK
#define PAM_FAIL_CHECK if (retval != PAM_SUCCESS) {goto error; }

	/* build and initialize the RADIUS packet */
	request->code = PW_AUTHENTICATION_REQUEST;
	get_random_vector(request->vector);
	request->id = request->vector[0]; /* this should be evenly distributed */

	/* grab the password (if any) from the previous authentication layer */
        if (!config.force_prompt) {
                DPRINT(LOG_DEBUG, "ignore last_pass, force_prompt set");
		retval = pam_get_item(pamh, PAM_AUTHTOK, (CONST void **) &password);
		PAM_FAIL_CHECK;
        }

	if (password) {
		password = strdup(password);
		DPRINT(LOG_DEBUG, "Got password %s", password);
	}

	/* no previous password: maybe get one from the user */
	if (!password) {
		if (ctrl & PAM_USE_FIRST_PASS) {
			retval = PAM_AUTH_ERR;	/* use one pass only, stopping if it fails */
			goto error;
		}

		/* check to see if we send a NULL password the first time around */
		if (!(ctrl & PAM_SKIP_PASSWD)) {
			retval = rad_converse(pamh, PAM_PROMPT_ECHO_OFF, config.prompt, &password);
			PAM_FAIL_CHECK;

		}
	} /* end of password == NULL */

	build_radius_packet(request, user, password, &config);
	/* not all servers understand this service type, but some do */
	add_int_attribute(request, PW_USER_SERVICE_TYPE, PW_AUTHENTICATE_ONLY);

	/*
	 *	Tell the server which host the user is coming from.
	 *
	 *	Note that this is NOT the IP address of the machine running PAM!
	 *	It's the IP address of the client.
	 */
	retval = pam_get_item(pamh, PAM_RHOST, (CONST void **) &rhost);
	PAM_FAIL_CHECK;
	if (rhost) {
		add_attribute(request, PW_CALLING_STATION_ID, (unsigned char *) rhost,
			strlen(rhost));
	}

	DPRINT(LOG_DEBUG, "Sending RADIUS request code %d", request->code);

	retval = talk_radius(&config, request, response, password, NULL, config.retries + 1);
	PAM_FAIL_CHECK;

	DPRINT(LOG_DEBUG, "Got RADIUS response code %d", response->code);

	/*
	 *	If we get an authentication failure, and we sent a NULL password,
	 *	ask the user for one and continue.
	 *
	 *	If we get an access challenge, then do a response, for as many
	 *	challenges as we receive.
	 */
	while (response->code == PW_ACCESS_CHALLENGE) {
		attribute_t *a_state, *a_reply;
		char challenge[BUFFER_SIZE];

		/* Now we do a bit more work: challenge the user, and get a response */
		if (((a_state = find_attribute(response, PW_STATE)) == NULL) ||
		    ((a_reply = find_attribute(response, PW_REPLY_MESSAGE)) == NULL)) {
			/* Actually, State isn't required. */
			_pam_log(LOG_ERR, "RADIUS Access-Challenge received with State or Reply-Message missing");
			retval = PAM_AUTHINFO_UNAVAIL;
			goto error;
		}

		/*
		 *	Security fixes.
		 */
		if ((a_state->length <= 2) || (a_reply->length <= 2)) {
			_pam_log(LOG_ERR, "RADIUS Access-Challenge received with invalid State or Reply-Message");
			retval = PAM_AUTHINFO_UNAVAIL;
			goto error;
		}

		memcpy(challenge, a_reply->data, a_reply->length - 2);
		challenge[a_reply->length - 2] = 0;

		/* It's full challenge-response, we should have echo on */
		retval = rad_converse(pamh, PAM_PROMPT_ECHO_ON, challenge, &resp2challenge);
		PAM_FAIL_CHECK;

		/* now that we've got a response, build a new radius packet */
		build_radius_packet(request, user, resp2challenge, &config);
		/* request->code is already PW_AUTHENTICATION_REQUEST */
		request->id++;		/* one up from the request */

		if (rhost) {
			add_attribute(request, PW_CALLING_STATION_ID, (unsigned char *) rhost,
				      strlen(rhost));
		}

		/* copy the state over from the servers response */
		add_attribute(request, PW_STATE, a_state->data, a_state->length - 2);

		retval = talk_radius(&config, request, response, resp2challenge, NULL, 1);
		PAM_FAIL_CHECK;

		DPRINT(LOG_DEBUG, "Got response to challenge code %d", response->code);

		/*
		 * max_challenge limits the # of challenges a server can issue
		 * It's a workaround for buggy servers
		 */
		if (config.max_challenge > 0 && response->code == PW_ACCESS_CHALLENGE) {
			num_challenge++;
			if (num_challenge >= config.max_challenge) {
				DPRINT(LOG_DEBUG, "maximum number of challenges (%d) reached, failing", num_challenge);
				break;
			}
		}
	}

	/* Whew! Done the pasword checks, look for an authentication acknowledge */
	if (response->code == PW_AUTHENTICATION_ACK) {
		retval = PAM_SUCCESS;
	} else {
		retval = PAM_AUTH_ERR;	/* authentication failure */

error:
		/* If there was a password pass it to the next layer */
		if (password && *password) {
			pam_set_item(pamh, PAM_AUTHTOK, password);
		}
	}

	DPRINT(LOG_DEBUG, "authentication %s", retval==PAM_SUCCESS ? "succeeded":"failed");

	close(config.sockfd);
	cleanup(config.server);
	_pam_forget(password);
	_pam_forget(resp2challenge);
	{
		int *pret = malloc(sizeof(int));
		*pret = retval;
		pam_set_data(pamh, "rad_setcred_return", (void *) pret, _int_free);
	}
	return retval;
}