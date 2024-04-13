PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, CONST char **argv)
{
	CONST char *user;
	char *password = NULL;
	char *new_password = NULL;
	char *check_password = NULL;
	int ctrl;
	int retval = PAM_AUTHTOK_ERR;
	int attempts;

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
		return PAM_USER_UNKNOWN;
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

	/* grab the old password (if any) from the previous password layer */
	retval = pam_get_item(pamh, PAM_OLDAUTHTOK, (CONST void **) &password);
	PAM_FAIL_CHECK;
	if (password) password = strdup(password);

	/* grab the new password (if any) from the previous password layer */
	retval = pam_get_item(pamh, PAM_AUTHTOK, (CONST void **) &new_password);
	PAM_FAIL_CHECK;
	if (new_password) new_password = strdup(new_password);

	/* preliminary password change checks. */
	if (flags & PAM_PRELIM_CHECK) {
		if (!password) {		/* no previous password: ask for one */
			retval = rad_converse(pamh, PAM_PROMPT_ECHO_OFF, config.prompt, &password);
			PAM_FAIL_CHECK;
		}

		/*
		 * We now check the password to see if it's the right one.
		 * If it isn't, we let the user try again.
		 * Note that RADIUS doesn't have any concept of 'root'.	The only way
		 * that root can change someone's password is to log into the RADIUS
		 * server, and and change it there.
		 */

		/* build and initialize the access request RADIUS packet */
		request->code = PW_AUTHENTICATION_REQUEST;
		get_random_vector(request->vector);
		request->id = request->vector[0]; /* this should be evenly distributed */

		build_radius_packet(request, user, password, &config);
		add_int_attribute(request, PW_USER_SERVICE_TYPE, PW_AUTHENTICATE_ONLY);

		retval = talk_radius(&config, request, response, password, NULL, 1);
		PAM_FAIL_CHECK;

		/* oops! They don't have the right password.	Complain and die. */
		if (response->code != PW_AUTHENTICATION_ACK) {
			_pam_forget(password);
			retval = PAM_PERM_DENIED;
			goto error;
		}

		/*
		 * We're now sure it's the right user.
		 * Ask for their new password, if appropriate
		 */

		if (!new_password) {	/* not found yet: ask for it */
			int new_attempts;
			attempts = 0;

			/* loop, trying to get matching new passwords */
			while (attempts++ < 3) {

				/* loop, trying to get a new password */
				new_attempts = 0;
				while (new_attempts++ < 3) {
					retval = rad_converse(pamh, PAM_PROMPT_ECHO_OFF,
							"New password: ", &new_password);
					PAM_FAIL_CHECK;

					/* the old password may be short.	Check it, first. */
					if (strcmp(password, new_password) == 0) { /* are they the same? */
						rad_converse(pamh, PAM_ERROR_MSG,
						 "You must choose a new password.", NULL);
						_pam_forget(new_password);
						continue;
					} else if (strlen(new_password) < 6) {
						rad_converse(pamh, PAM_ERROR_MSG, "it's WAY too short", NULL);
						_pam_forget(new_password);
						continue;
					}

					/* insert crypt password checking here */

					break;		/* the new password is OK */
				}

				if (new_attempts >= 3) { /* too many new password attempts: die */
					retval = PAM_AUTHTOK_ERR;
					goto error;
				}

				/* make sure of the password by asking for verification */
				retval = rad_converse(pamh, PAM_PROMPT_ECHO_OFF,
						      "New password (again): ", &check_password);
				PAM_FAIL_CHECK;

				retval = strcmp(new_password, check_password);
				_pam_forget(check_password);

				/* if they don't match, don't pass them to the next module */
				if (retval != 0) {
					_pam_forget(new_password);
					rad_converse(pamh, PAM_ERROR_MSG,
								 "You must enter the same password twice.", NULL);
					retval = PAM_AUTHTOK_ERR;
					goto error;		/* ??? maybe this should be a 'continue' ??? */
				}

				break;			/* everything's fine */
			}	/* loop, trying to get matching new passwords */

			if (attempts >= 3) { /* too many new password attempts: die */
				retval = PAM_AUTHTOK_ERR;
				goto error;
			}
		} /* now we have a new password which passes all of our tests */

		/*
		 * Solaris 2.6 calls pam_sm_chauthtok only ONCE, with PAM_PRELIM_CHECK
		 * set.
		 */
#ifndef sun
		/* If told to update the authentication token, do so. */
	} else if (flags & PAM_UPDATE_AUTHTOK) {
#endif

		if (!password || !new_password) { /* ensure we've got passwords */
			retval = PAM_AUTHTOK_ERR;
			goto error;
		}

		/* build and initialize the password change request RADIUS packet */
		request->code = PW_PASSWORD_REQUEST;
		get_random_vector(request->vector);
		request->id = request->vector[0]; /* this should be evenly distributed */

		/* the secret here can not be know to the user, so it's the new password */
		_pam_forget(config.server->secret);
		config.server->secret = strdup(password); /* it's free'd later */

		build_radius_packet(request, user, new_password, &config);
		add_password(request, PW_OLD_PASSWORD, password, password);

		retval = talk_radius(&config, request, response, new_password, password, 1);
		PAM_FAIL_CHECK;

		/* Whew! Done password changing, check for password acknowledge */
		if (response->code != PW_PASSWORD_ACK) {
			retval = PAM_AUTHTOK_ERR;
			goto error;
		}
	}

	/*
	 * Send the passwords to the next stage if preliminary checks fail,
	 * or if the password change request fails.
	 */
	if ((flags & PAM_PRELIM_CHECK) || (retval != PAM_SUCCESS)) {
	error:

		/* If there was a password pass it to the next layer */
		if (password && *password) {
			pam_set_item(pamh, PAM_OLDAUTHTOK, password);
		}

		if (new_password && *new_password) {
			pam_set_item(pamh, PAM_AUTHTOK, new_password);
		}
	}

	if (ctrl & PAM_DEBUG_ARG) {
		_pam_log(LOG_DEBUG, "password change %s", retval==PAM_SUCCESS ? "succeeded" : "failed");
	}

	close(config.sockfd);
	cleanup(config.server);

	_pam_forget(password);
	_pam_forget(new_password);
	return retval;
}