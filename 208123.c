load_modules(pam_handle_t *pamh, int type, char *function_name,
    pamtab_t *pam_entry)
{
	void	*mh;
	struct	auth_module *authp;
	struct	account_module *accountp;
	struct	session_module *sessionp;
	struct	password_module *passwdp;
	int	loading_functions = 0; /* are we currently loading functions? */

	pam_trace(PAM_DEBUG_MODULE, "load_modules[%d:%s](%p, %s)=%s:%s",
	    pamh->include_depth, pam_trace_cname(pamh), (void *)pamh,
	    function_name, pam_trace_fname(pam_entry->pam_flag),
	    pam_entry->module_path);

	while (pam_entry != NULL) {
		pam_trace(PAM_DEBUG_DEFAULT,
		    "while load_modules[%d:%s](%p, %s)=%s",
		    pamh->include_depth, pam_trace_cname(pamh), (void *)pamh,
		    function_name, pam_entry->module_path);

		if (pam_entry->pam_flag & PAM_INCLUDE) {
			pam_trace(PAM_DEBUG_DEFAULT,
			    "done load_modules[%d:%s](%p, %s)=%s",
			    pamh->include_depth, pam_trace_cname(pamh),
			    (void *)pamh, function_name,
			    pam_entry->module_path);
			return (PAM_SUCCESS);
		}
		switch (type) {
		case PAM_AUTH_MODULE:

			/* if the function has already been loaded, return */
			authp = pam_entry->function_ptr;
			if (!loading_functions &&
			    (((strcmp(function_name, PAM_SM_AUTHENTICATE)
			    == 0) && authp && authp->pam_sm_authenticate) ||
			    ((strcmp(function_name, PAM_SM_SETCRED) == 0) &&
			    authp && authp->pam_sm_setcred))) {
				return (PAM_SUCCESS);
			}

			/* function has not been loaded yet */
			loading_functions = 1;
			if (authp == NULL) {
				authp = calloc(1, sizeof (struct auth_module));
				if (authp == NULL)
					return (PAM_BUF_ERR);
			}

			/* if open_module fails, return error */
			if ((mh = open_module(pamh,
			    pam_entry->module_path)) == NULL) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "load_modules[%d:%s]: can not open module "
				    "%s", pamh->include_depth,
				    pam_trace_cname(pamh),
				    pam_entry->module_path);
				free(authp);
				return (PAM_OPEN_ERR);
			}

			/* load the authentication function */
			if (strcmp(function_name, PAM_SM_AUTHENTICATE) == 0) {
				if (load_function(mh, PAM_SM_AUTHENTICATE,
				    &authp->pam_sm_authenticate)
				    != PAM_SUCCESS) {
					/* return error if dlsym fails */
					free(authp);
					return (PAM_SYMBOL_ERR);
				}

			/* load the setcred function */
			} else if (strcmp(function_name, PAM_SM_SETCRED) == 0) {
				if (load_function(mh, PAM_SM_SETCRED,
				    &authp->pam_sm_setcred) != PAM_SUCCESS) {
					/* return error if dlsym fails */
					free(authp);
					return (PAM_SYMBOL_ERR);
				}
			}
			pam_entry->function_ptr = authp;
			break;
		case PAM_ACCOUNT_MODULE:
			accountp = pam_entry->function_ptr;
			if (!loading_functions &&
			    (strcmp(function_name, PAM_SM_ACCT_MGMT) == 0) &&
			    accountp && accountp->pam_sm_acct_mgmt) {
				return (PAM_SUCCESS);
			}

			/*
			 * If functions are added to the account module,
			 * verify that one of the other functions hasn't
			 * already loaded it.  See PAM_AUTH_MODULE code.
			 */
			loading_functions = 1;
			accountp = calloc(1, sizeof (struct account_module));
			if (accountp == NULL)
				return (PAM_BUF_ERR);

			/* if open_module fails, return error */
			if ((mh = open_module(pamh,
			    pam_entry->module_path)) == NULL) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "load_modules[%d:%s]: can not open module "
				    "%s", pamh->include_depth,
				    pam_trace_cname(pamh),
				    pam_entry->module_path);
				free(accountp);
				return (PAM_OPEN_ERR);
			}

			if (load_function(mh, PAM_SM_ACCT_MGMT,
			    &accountp->pam_sm_acct_mgmt) != PAM_SUCCESS) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "load_modules[%d:%s]: pam_sm_acct_mgmt() "
				    "missing", pamh->include_depth,
				    pam_trace_cname(pamh));
				free(accountp);
				return (PAM_SYMBOL_ERR);
			}
			pam_entry->function_ptr = accountp;
			break;
		case PAM_SESSION_MODULE:
			sessionp = pam_entry->function_ptr;
			if (!loading_functions &&
			    (((strcmp(function_name,
			    PAM_SM_OPEN_SESSION) == 0) &&
			    sessionp && sessionp->pam_sm_open_session) ||
			    ((strcmp(function_name,
			    PAM_SM_CLOSE_SESSION) == 0) &&
			    sessionp && sessionp->pam_sm_close_session))) {
				return (PAM_SUCCESS);
			}

			loading_functions = 1;
			if (sessionp == NULL) {
				sessionp = calloc(1,
				    sizeof (struct session_module));
				if (sessionp == NULL)
					return (PAM_BUF_ERR);
			}

			/* if open_module fails, return error */
			if ((mh = open_module(pamh,
			    pam_entry->module_path)) == NULL) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "load_modules[%d:%s]: can not open module "
				    "%s", pamh->include_depth,
				    pam_trace_cname(pamh),
				    pam_entry->module_path);
				free(sessionp);
				return (PAM_OPEN_ERR);
			}

			if ((strcmp(function_name, PAM_SM_OPEN_SESSION) == 0) &&
			    load_function(mh, PAM_SM_OPEN_SESSION,
			    &sessionp->pam_sm_open_session) != PAM_SUCCESS) {
				free(sessionp);
				return (PAM_SYMBOL_ERR);
			} else if ((strcmp(function_name,
			    PAM_SM_CLOSE_SESSION) == 0) &&
			    load_function(mh, PAM_SM_CLOSE_SESSION,
			    &sessionp->pam_sm_close_session) != PAM_SUCCESS) {
				free(sessionp);
				return (PAM_SYMBOL_ERR);
			}
			pam_entry->function_ptr = sessionp;
			break;
		case PAM_PASSWORD_MODULE:
			passwdp = pam_entry->function_ptr;
			if (!loading_functions &&
			    (strcmp(function_name, PAM_SM_CHAUTHTOK) == 0) &&
			    passwdp && passwdp->pam_sm_chauthtok) {
				return (PAM_SUCCESS);
			}

			/*
			 * If functions are added to the password module,
			 * verify that one of the other functions hasn't
			 * already loaded it.  See PAM_AUTH_MODULE code.
			 */
			loading_functions = 1;
			passwdp = calloc(1, sizeof (struct password_module));
			if (passwdp == NULL)
				return (PAM_BUF_ERR);

			/* if open_module fails, continue */
			if ((mh = open_module(pamh,
			    pam_entry->module_path)) == NULL) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "load_modules[%d:%s]: can not open module "
				    "%s", pamh->include_depth,
				    pam_trace_cname(pamh),
				    pam_entry->module_path);
				free(passwdp);
				return (PAM_OPEN_ERR);
			}

			if (load_function(mh, PAM_SM_CHAUTHTOK,
			    &passwdp->pam_sm_chauthtok) != PAM_SUCCESS) {
				free(passwdp);
				return (PAM_SYMBOL_ERR);
			}
			pam_entry->function_ptr = passwdp;
			break;
		default:
			pam_trace(PAM_DEBUG_DEFAULT,
			    "load_modules[%d:%s](%p, %s): unsupported type %d",
			    pamh->include_depth, pam_trace_cname(pamh),
			    (void *)pamh, function_name, type);
			break;
		}

		pam_entry = pam_entry->next;
	} /* while */

	pam_trace(PAM_DEBUG_MODULE, "load_modules[%d:%s](%p, %s)=done",
	    pamh->include_depth, pam_trace_cname(pamh), (void *)pamh,
	    function_name);

	return (PAM_SUCCESS);
}