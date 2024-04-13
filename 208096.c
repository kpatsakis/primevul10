pam_authenticate(pam_handle_t *pamh, int flags)
{
	int	retval;

	retval = run_stack(pamh, flags, PAM_AUTH_MODULE, PAM_AUTH_ERR,
	    PAM_AUTHENTICATE, "pam_authenticate");

	if (retval != PAM_SUCCESS)
		(void) pam_set_item(pamh, PAM_AUTHTOK, NULL);
	return (retval);
}