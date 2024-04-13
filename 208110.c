__pam_get_authtok(pam_handle_t *pamh, int source, int type, char *prompt,
    char **authtok)
{
	int error = PAM_SYSTEM_ERR;
	char *new_password = NULL;
	struct pam_response *ret_resp = NULL;
	char messages[PAM_MAX_NUM_MSG][PAM_MAX_MSG_SIZE];

	if ((*authtok = calloc(PASS_MAX+1, sizeof (char))) == NULL)
		return (PAM_BUF_ERR);

	if (prompt == NULL)
		prompt = dgettext(TEXT_DOMAIN, "password: ");

	switch (source) {
	case PAM_HANDLE:

		/* get password from pam handle item list */

		switch (type) {
		case PAM_AUTHTOK:
		case PAM_OLDAUTHTOK:

			if ((error = pam_get_item(pamh, type,
			    (void **)&new_password)) != PAM_SUCCESS)
				goto err_ret;

			if (new_password == NULL || new_password[0] == '\0') {
				free(*authtok);
				*authtok = NULL;
			} else {
				(void) strlcpy(*authtok, new_password,
				    PASS_MAX+1);
			}
			break;
		default:
			__pam_log(LOG_AUTH | LOG_ERR,
			    "__pam_get_authtok() invalid type: %d", type);
			error = PAM_SYMBOL_ERR;
			goto err_ret;
		}
		break;
	case PAM_PROMPT:

		/*
		 * Prompt for new password and save in pam handle item list
		 * if the that item is not already set.
		 */

		(void) strncpy(messages[0], prompt, sizeof (messages[0]));
		if ((error = do_conv(pamh, PAM_PROMPT_ECHO_OFF, 1, messages,
		    NULL, &ret_resp)) != PAM_SUCCESS)
			goto err_ret;

		if (ret_resp->resp == NULL) {
			/* getpass didn't return anything */
			error = PAM_SYSTEM_ERR;
			goto err_ret;
		}

		/* save the new password if this item was NULL */
		if (type) {
			if ((error = pam_get_item(pamh, type,
			    (void **)&new_password)) != PAM_SUCCESS) {
				free_resp(1, ret_resp);
				goto err_ret;
			}
			if (new_password == NULL)
				(void) pam_set_item(pamh, type, ret_resp->resp);
		}

		(void) strlcpy(*authtok, ret_resp->resp, PASS_MAX+1);
		free_resp(1, ret_resp);
		break;
	default:
		__pam_log(LOG_AUTH | LOG_ERR,
		    "__pam_get_authtok() invalid source: %d", source);
		error = PAM_SYMBOL_ERR;
		goto err_ret;
	}

	return (PAM_SUCCESS);

err_ret:
	bzero(*authtok, PASS_MAX+1);
	free(*authtok);
	*authtok = NULL;
	return (error);
}