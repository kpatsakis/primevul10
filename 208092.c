pam_open_session(pam_handle_t *pamh, int flags)
{
	int	retval;

	retval = run_stack(pamh, flags, PAM_SESSION_MODULE, PAM_SESSION_ERR,
	    PAM_OPEN_SESSION, "pam_open_session");

	if (retval != PAM_SUCCESS)
		(void) pam_set_item(pamh, PAM_AUTHTOK, NULL);
	return (retval);
}