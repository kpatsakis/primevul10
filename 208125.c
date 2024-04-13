pam_get_user(pam_handle_t *pamh, char **user, const char *prompt_override)
{
	int	status;
	char	*prompt = NULL;
	char    *real_username;
	struct pam_response *ret_resp = NULL;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	pam_trace(PAM_DEBUG_DEFAULT,
	    "pam_get_user(%p, %p, %s)", (void *)pamh, (void *)*user,
	    prompt_override ? prompt_override : "NULL");
	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);

	if ((status = pam_get_item(pamh, PAM_USER, (void **)user))
	    != PAM_SUCCESS) {
		return (status);
	}

	/* if the user is set, return it */

	if (*user != NULL && *user[0] != '\0') {
		return (PAM_SUCCESS);
	}

	/*
	 * if the module is requesting a special prompt, use it.
	 * else use PAM_USER_PROMPT.
	 */

	if (prompt_override != NULL) {
		prompt = (char *)prompt_override;
	} else {
		status = pam_get_item(pamh, PAM_USER_PROMPT, (void**)&prompt);
		if (status != PAM_SUCCESS) {
			return (status);
		}
	}

	/* if the prompt is not set, use default */

	if (prompt == NULL || prompt[0] == '\0') {
		prompt = dgettext(TEXT_DOMAIN, "Please enter user name: ");
	}

	/* prompt for the user */

	(void) strncpy(messages[0], prompt, sizeof (messages[0]));

	for (;;) {
		int state = WHITESPACE;

		status = do_conv(pamh, PAM_PROMPT_ECHO_ON, 1, messages,
		    NULL, &ret_resp);

		if (status != PAM_SUCCESS) {
			return (status);
		}

		if (ret_resp->resp && ret_resp->resp[0] != '\0') {
			int len = strlen(ret_resp->resp);
			int i;

			for (i = 0; i < len; i++) {
				if ((ret_resp->resp[i] != ' ') &&
				    (ret_resp->resp[i] != '\t')) {
					state = USERNAME;
					break;
				}
			}

			if (state == USERNAME)
				break;
		}
		/* essentially empty response, try again */
		free_resp(1, ret_resp);
		ret_resp = NULL;
	}

	/* set PAM_USER */
	/* Parse the user input to get the user name. */
	status = parse_user_name(ret_resp->resp, &real_username);

	if (status != PAM_SUCCESS) {
		if (real_username != NULL)
			free(real_username);
		free_resp(1, ret_resp);
		return (status);
	}

	status = pam_set_item(pamh, PAM_USER, real_username);

	free(real_username);

	free_resp(1, ret_resp);
	if (status != PAM_SUCCESS) {
		return (status);
	}

	/*
	 * finally, get PAM_USER. We have to call pam_get_item to get
	 * the value of user because pam_set_item mallocs the memory.
	 */

	status = pam_get_item(pamh, PAM_USER, (void**)user);
	return (status);
}