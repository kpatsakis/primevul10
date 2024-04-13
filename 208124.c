pam_chauthtok(pam_handle_t *pamh, int flags)
{
	int	retval;

	/* do not let apps use PAM_PRELIM_CHECK or PAM_UPDATE_AUTHTOK */
	if (flags & (PAM_PRELIM_CHECK | PAM_UPDATE_AUTHTOK)) {
		pam_trace(PAM_DEBUG_DEFAULT,
		    "pam_chauthtok(%p, %x): %s", (void *)pamh, flags,
		    pam_strerror(pamh, PAM_SYMBOL_ERR));
		return (PAM_SYMBOL_ERR);
	}

	/* 1st pass: PRELIM CHECK */
	retval = run_stack(pamh, flags | PAM_PRELIM_CHECK, PAM_PASSWORD_MODULE,
	    PAM_AUTHTOK_ERR, PAM_CHAUTHTOK, "pam_chauthtok-prelim");

	if (retval == PAM_TRY_AGAIN)
		return (retval);

	if (retval != PAM_SUCCESS) {
		(void) pam_set_item(pamh, PAM_AUTHTOK, NULL);
		return (retval);
	}

	/* 2nd pass: UPDATE AUTHTOK */
	retval = run_stack(pamh, flags | PAM_UPDATE_AUTHTOK,
	    PAM_PASSWORD_MODULE, PAM_AUTHTOK_ERR, PAM_CHAUTHTOK,
	    "pam_chauthtok-update");

	if (retval != PAM_SUCCESS)
		(void) pam_set_item(pamh, PAM_AUTHTOK, NULL);

	return (retval);
}