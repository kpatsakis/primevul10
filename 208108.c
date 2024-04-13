get_pam_conf_entry(struct pam_fh *pam_fh, pam_handle_t *pamh, pamtab_t **pam)
{
	char		*cp, *arg;
	int		argc;
	char		*tmp, *tmp_free;
	int		i;
	char		*current_line = NULL;
	int		error = PAM_SYSTEM_ERR;	/* preset to error */
	int		err;

	/* get the next line from pam.conf */
	if ((cp = nextline(pam_fh, pamh, &err)) == NULL) {
		/* no more lines in pam.conf ==> return */
		error = PAM_SUCCESS;
		*pam = NULL;
		goto out;
	}

	if ((*pam = calloc(1, sizeof (pamtab_t))) == NULL) {
		__pam_log(LOG_AUTH | LOG_ERR, "strdup: out of memory");
		goto out;
	}

	/* copy full line for error reporting */
	if ((current_line = strdup(cp)) == NULL) {
		__pam_log(LOG_AUTH | LOG_ERR, "strdup: out of memory");
		goto out;
	}

	pam_trace(PAM_DEBUG_CONF,
	    "pam.conf[%s] entry:\t%s", pam_trace_cname(pamh), current_line);

	/* get service name (e.g. login, su, passwd) */
	if ((arg = read_next_token(&cp)) == 0) {
		__pam_log(LOG_AUTH | LOG_CRIT,
		    "illegal pam.conf[%s] entry: %s: missing SERVICE NAME",
		    pam_trace_cname(pamh), current_line);
		goto out;
	}
	if (((*pam)->pam_service = strdup(arg)) == 0) {
		__pam_log(LOG_AUTH | LOG_ERR, "strdup: out of memory");
		goto out;
	}

	/* get module type (e.g. authentication, acct mgmt) */
	if ((arg = read_next_token(&cp)) == 0) {
		__pam_log(LOG_AUTH | LOG_CRIT,
		    "illegal pam.conf[%s] entry: %s: missing MODULE TYPE",
		    pam_trace_cname(pamh), current_line);
		(*pam)->pam_type = -1;	/* 0 is a valid value */
		goto getflag;
	}
	if (strcasecmp(arg, PAM_AUTH_NAME) == 0) {
		(*pam)->pam_type = PAM_AUTH_MODULE;
	} else if (strcasecmp(arg, PAM_ACCOUNT_NAME) == 0) {
		(*pam)->pam_type = PAM_ACCOUNT_MODULE;
	} else if (strcasecmp(arg, PAM_SESSION_NAME) == 0) {
		(*pam)->pam_type = PAM_SESSION_MODULE;
	} else if (strcasecmp(arg, PAM_PASSWORD_NAME) == 0) {
		(*pam)->pam_type = PAM_PASSWORD_MODULE;
	} else {
		/* error */
		__pam_log(LOG_AUTH | LOG_CRIT,
		    "illegal pam.conf[%s] entry: %s: invalid module "
		    "type: %s", pam_trace_cname(pamh), current_line, arg);
		(*pam)->pam_type = -1;	/* 0 is a valid value */
	}

getflag:
	/* get pam flag (e.g., requisite, required, sufficient, optional) */
	if ((arg = read_next_token(&cp)) == 0) {
		__pam_log(LOG_AUTH | LOG_CRIT,
		    "illegal pam.conf[%s] entry: %s: missing CONTROL FLAG",
		    pam_trace_cname(pamh), current_line);
		goto getpath;
	}
	if (strcasecmp(arg, PAM_BINDING_NAME) == 0) {
		(*pam)->pam_flag = PAM_BINDING;
	} else if (strcasecmp(arg, PAM_INCLUDE_NAME) == 0) {
		(*pam)->pam_flag = PAM_INCLUDE;
	} else if (strcasecmp(arg, PAM_OPTIONAL_NAME) == 0) {
		(*pam)->pam_flag = PAM_OPTIONAL;
	} else if (strcasecmp(arg, PAM_REQUIRED_NAME) == 0) {
		(*pam)->pam_flag = PAM_REQUIRED;
	} else if (strcasecmp(arg, PAM_REQUISITE_NAME) == 0) {
		(*pam)->pam_flag = PAM_REQUISITE;
	} else if (strcasecmp(arg, PAM_SUFFICIENT_NAME) == 0) {
		(*pam)->pam_flag = PAM_SUFFICIENT;
	} else {
		/* error */
		__pam_log(LOG_AUTH | LOG_CRIT,
		    "illegal pam.conf[%s] entry: %s",
		    pam_trace_cname(pamh), current_line);
		__pam_log(LOG_AUTH | LOG_CRIT,
		    "\tinvalid control flag: %s", arg);
	}

getpath:
	/* get module path (e.g. /usr/lib/security/pam_unix_auth.so.1) */
	if ((arg = read_next_token(&cp)) == 0) {
		__pam_log(LOG_AUTH | LOG_CRIT,
		    "illegal pam.conf[%s] entry: %s: missing MODULE PATH",
		    pam_trace_cname(pamh), current_line);
		error = PAM_SUCCESS;	/* success */
		goto out;
	}
	if (arg[0] != '/') {
		size_t len;
		/*
		 * If module path does not start with "/", then
		 * prepend PAM_LIB_DIR (/usr/lib/security/).
		 */
		/* sizeof (PAM_LIB_DIR) has room for '\0' */
		len = sizeof (PAM_LIB_DIR) + sizeof (PAM_ISA_DIR) + strlen(arg);
		if (((*pam)->module_path = malloc(len)) == NULL) {
			__pam_log(LOG_AUTH | LOG_ERR, "strdup: out of memory");
			goto out;
		}
		if ((*pam)->pam_flag & PAM_INCLUDE) {
			(void) snprintf((*pam)->module_path, len, "%s%s",
			    PAM_LIB_DIR, arg);
		} else {
			(void) snprintf((*pam)->module_path, len, "%s%s%s",
			    PAM_LIB_DIR, PAM_ISA_DIR, arg);
		}
	} else {
		/* Full path provided for module */
		char *isa;

		/* Check for Instruction Set Architecture indicator */
		if ((isa = strstr(arg, PAM_ISA)) != NULL) {
			size_t len;
			len = strlen(arg) - (sizeof (PAM_ISA)-1) +
			    sizeof (PAM_ISA_DIR);

			/* substitute the architecture dependent path */
			if (((*pam)->module_path = malloc(len)) == NULL) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "strdup: out of memory");
				goto out;
			}
			*isa = '\000';
			isa += strlen(PAM_ISA);
			(void) snprintf((*pam)->module_path, len, "%s%s%s",
			    arg, PAM_ISA_DIR, isa);
		} else if (((*pam)->module_path = strdup(arg)) == 0) {
			__pam_log(LOG_AUTH | LOG_ERR, "strdup: out of memory");
			goto out;
		}
	}

	/* count the number of module-specific options first */
	argc = 0;
	if ((tmp = strdup(cp)) == NULL) {
		__pam_log(LOG_AUTH | LOG_ERR, "strdup: out of memory");
		goto out;
	}
	tmp_free = tmp;
	for (arg = read_next_token(&tmp); arg; arg = read_next_token(&tmp))
		argc++;
	free(tmp_free);

	/* allocate array for the module-specific options */
	if (argc > 0) {
		if (((*pam)->module_argv =
		    calloc(argc+1, sizeof (char *))) == 0) {
			__pam_log(LOG_AUTH | LOG_ERR, "calloc: out of memory");
			goto out;
		}
		i = 0;
		for (arg = read_next_token(&cp); arg;
		    arg = read_next_token(&cp)) {
			(*pam)->module_argv[i] = strdup(arg);
			if ((*pam)->module_argv[i] == NULL) {
				__pam_log(LOG_AUTH | LOG_ERR, "strdup failed");
				goto out;
			}
			i++;
		}
		(*pam)->module_argv[argc] = NULL;
	}
	(*pam)->module_argc = argc;

	error = PAM_SUCCESS;	/* success */
	(*pam)->pam_err = err;	/* was the line truncated */

out:
	if (current_line)
		free(current_line);
	if (error != PAM_SUCCESS) {
		/* on error free this */
		if (*pam)
			free_pamconf(*pam);
	}
	return (error);
}