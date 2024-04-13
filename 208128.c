run_stack(pam_handle_t *pamh, int flags, int type, int def_err, int ind,
    char *function_name)
{
	int	err = PAM_SYSTEM_ERR;  /* preset */
	int	optional_error = 0;
	int	required_error = 0;
	int	success = 0;
	pamtab_t *modulep;
	int	(*sm_func)();

	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);

	/* read initial entries from pam.conf */
	if ((err = read_pam_conf(pamh, PAM_CONFIG)) != PAM_SUCCESS) {
		return (err);
	}

	if ((modulep =
	    pamh->pam_conf_info[pamh->include_depth][type]) == NULL) {
		__pam_log(LOG_AUTH | LOG_ERR, "%s no initial module present",
		    pam_trace_cname(pamh));
		goto exit_return;
	}

	pamh->pam_inmodule = WO_OK;	/* OK to get AUTHTOK */
include:
	pam_trace(PAM_DEBUG_MODULE,
	    "[%d:%s]:run_stack:%s(%p, %x): %s", pamh->include_depth,
	    pam_trace_cname(pamh), function_name, (void *)pamh, flags,
	    modulep ? modulep->module_path : "NULL");

	while (modulep != NULL) {
		if (modulep->pam_flag & PAM_INCLUDE) {
			/* save the return location */
			pamh->pam_conf_modulep[pamh->include_depth] =
			    modulep->next;
			pam_trace(PAM_DEBUG_MODULE,
			    "setting for include[%d:%p]",
			    pamh->include_depth, (void *)modulep->next);
			if (pamh->include_depth++ >= PAM_MAX_INCLUDE) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "run_stack: includes too deep %d "
				    "found trying to include %s from %s, %d "
				    "allowed", pamh->include_depth,
				    modulep->module_path, pamh->pam_conf_name
				    [PAM_MAX_INCLUDE] == NULL ? "NULL" :
				    pamh->pam_conf_name[PAM_MAX_INCLUDE],
				    PAM_MAX_INCLUDE);
				goto exit_return;
			}
			if ((err = read_pam_conf(pamh,
			    modulep->module_path)) != PAM_SUCCESS) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "run_stack[%d:%s]: can't read included "
				    "conf %s", pamh->include_depth,
				    pam_trace_cname(pamh),
				    modulep->module_path);
				goto exit_return;
			}
			if ((modulep = pamh->pam_conf_info
			    [pamh->include_depth][type]) == NULL) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "run_stack[%d:%s]: no include module "
				    "present %s", pamh->include_depth,
				    pam_trace_cname(pamh), function_name);
				goto exit_return;
			}
			if (modulep->pam_flag & PAM_INCLUDE) {
				/* first line another include */
				goto include;
			}
			pam_trace(PAM_DEBUG_DEFAULT, "include[%d:%s]"
			    "(%p, %s)=%s", pamh->include_depth,
			    pam_trace_cname(pamh), (void *)pamh,
			    function_name, modulep->module_path);
			if ((err = load_modules(pamh, type, sm_name(ind),
			    pamh->pam_conf_info
			    [pamh->include_depth][type])) != PAM_SUCCESS) {
				pam_trace(PAM_DEBUG_DEFAULT,
				    "[%d:%s]:%s(%p, %x): load_modules failed",
				    pamh->include_depth, pam_trace_cname(pamh),
				    function_name, (void *)pamh, flags);
				goto exit_return;
			}
			if ((modulep = pamh->pam_conf_info
			    [pamh->include_depth][type]) == NULL) {
				__pam_log(LOG_AUTH | LOG_ERR,
				    "%s no initial module present",
				    pam_trace_cname(pamh));
				goto exit_return;
			}
		} else if ((err = load_modules(pamh, type, sm_name(ind),
		    modulep)) != PAM_SUCCESS) {
			pam_trace(PAM_DEBUG_DEFAULT,
			    "[%d:%s]:%s(%p, %x): load_modules failed",
			    pamh->include_depth, pam_trace_cname(pamh),
			    function_name, (void *)pamh, flags);
			goto exit_return;
		}  /* PAM_INCLUDE */
		sm_func = func(modulep, ind);
		if (sm_func) {
			err = sm_func(pamh, flags, modulep->module_argc,
			    (const char **)modulep->module_argv);

			pam_trace(PAM_DEBUG_MODULE,
			    "[%d:%s]:%s(%p, %x): %s returned %s",
			    pamh->include_depth, pam_trace_cname(pamh),
			    function_name, (void *)pamh, flags,
			    modulep->module_path, pam_strerror(pamh, err));

			switch (err) {
			case PAM_IGNORE:
				/* do nothing */
				break;
			case PAM_SUCCESS:
				if ((modulep->pam_flag & PAM_SUFFI_BIND) &&
				    !required_error) {
					pamh->pam_inmodule = RW_OK;
					pam_trace(PAM_DEBUG_MODULE,
					    "[%d:%s]:%s(%p, %x): %s: success",
					    pamh->include_depth,
					    pam_trace_cname(pamh),
					    function_name, (void *)pamh, flags,
					    (modulep->pam_flag & PAM_BINDING) ?
					    PAM_BINDING_NAME :
					    PAM_SUFFICIENT_NAME);
					goto exit_return;
				}
				success = 1;
				break;
			case PAM_TRY_AGAIN:
				/*
				 * We need to return immediately, and
				 * we shouldn't reset the AUTHTOK item
				 * since it is not an error per-se.
				 */
				pamh->pam_inmodule = RW_OK;
				pam_trace(PAM_DEBUG_MODULE,
				    "[%d:%s]:%s(%p, %x): TRY_AGAIN: %s",
				    pamh->include_depth, pam_trace_cname(pamh),
				    function_name, (void *)pamh, flags,
				    pam_strerror(pamh, required_error ?
				    required_error : err));
				err = required_error ? required_error : err;
				goto exit_return;
			default:
				if (modulep->pam_flag & PAM_REQUISITE) {
					pamh->pam_inmodule = RW_OK;
					pam_trace(PAM_DEBUG_MODULE,
					    "[%d:%s]:%s(%p, %x): requisite: %s",
					    pamh->include_depth,
					    pam_trace_cname(pamh),
					    function_name, (void *)pamh, flags,
					    pam_strerror(pamh,
					    required_error ? required_error :
					    err));
					err = required_error ?
					    required_error : err;
					goto exit_return;
				} else if (modulep->pam_flag & PAM_REQRD_BIND) {
					if (!required_error)
						required_error = err;
				} else {
					if (!optional_error)
						optional_error = err;
				}
				pam_trace(PAM_DEBUG_DEFAULT,
				    "[%d:%s]:%s(%p, %x): error %s",
				    pamh->include_depth, pam_trace_cname(pamh),
				    function_name, (void *)pamh, flags,
				    pam_strerror(pamh, err));
				break;
			}
		}
		modulep = modulep->next;
	}

	pam_trace(PAM_DEBUG_MODULE, "[%d:%s]:stack_end:%s(%p, %x): %s %s: %s",
	    pamh->include_depth, pam_trace_cname(pamh), function_name,
	    (void *)pamh, flags, pamh->include_depth ? "included" : "final",
	    required_error ? "required" : success ? "success" :
	    optional_error ? "optional" : "default",
	    pam_strerror(pamh, required_error ? required_error :
	    success ? PAM_SUCCESS : optional_error ? optional_error : def_err));
	if (pamh->include_depth > 0) {
		free_pam_conf_info(pamh);
		pamh->include_depth--;
		/* continue at next entry */
		modulep = pamh->pam_conf_modulep[pamh->include_depth];
		pam_trace(PAM_DEBUG_MODULE, "looping for include[%d:%p]",
		    pamh->include_depth, (void *)modulep);
		goto include;
	}
	free_pam_conf_info(pamh);
	pamh->pam_inmodule = RW_OK;
	if (required_error != 0)
		return (required_error);
	else if (success != 0)
		return (PAM_SUCCESS);
	else if (optional_error != 0)
		return (optional_error);
	else
		return (def_err);

exit_return:
	/*
	 * All done at whatever depth we're at.
	 * Go back to not having read /etc/pam.conf
	 */
	while (pamh->include_depth > 0) {
		free_pam_conf_info(pamh);
		pamh->include_depth--;
	}
	free_pam_conf_info(pamh);
	pamh->pam_inmodule = RW_OK;
	return (err);
}