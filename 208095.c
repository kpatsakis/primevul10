pam_setcred(pam_handle_t *pamh, int flags)
{
	int	retval;

	retval = run_stack(pamh, flags, PAM_AUTH_MODULE, PAM_CRED_ERR,
	    PAM_SETCRED, "pam_setcred");

	if (retval != PAM_SUCCESS)
		(void) pam_set_item(pamh, PAM_AUTHTOK, NULL);
	return (retval);
}