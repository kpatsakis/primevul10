static int pam_private_session(pam_handle_t *pamh, int flags, int argc, CONST char **argv, int status)
{
	CONST char *user;
	int ctrl;
	int retval = PAM_AUTH_ERR;

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
	retval = initialize(&config, TRUE);
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
	request->code = PW_ACCOUNTING_REQUEST;
	get_random_vector(request->vector);
	request->id = request->vector[0]; /* this should be evenly distributed */

	build_radius_packet(request, user, NULL, &config);

	add_int_attribute(request, PW_ACCT_STATUS_TYPE, status);

	sprintf(recv_buffer, "%08d", (int) getpid());
	add_attribute(request, PW_ACCT_SESSION_ID, (unsigned char *) recv_buffer, strlen(recv_buffer));

	add_int_attribute(request, PW_ACCT_AUTHENTIC, PW_AUTH_RADIUS);

	if (status == PW_STATUS_START) {
		session_time = time(NULL);
	} else {
		add_int_attribute(request, PW_ACCT_SESSION_TIME, time(NULL) - session_time);
	}

	retval = talk_radius(&config, request, response, NULL, NULL, 1);
	PAM_FAIL_CHECK;

	/* oops! They don't have the right password.	Complain and die. */
	if (response->code != PW_ACCOUNTING_RESPONSE) {
		retval = PAM_PERM_DENIED;
		goto error;
	}

	retval = PAM_SUCCESS;

error:

	close(config.sockfd);
	cleanup(config.server);

	return retval;
}