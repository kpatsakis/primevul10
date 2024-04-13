pam_acct_mgmt(pam_handle_t *pamh, int flags)
{
	int	retval;

	retval = run_stack(pamh, flags, PAM_ACCOUNT_MODULE, PAM_ACCT_EXPIRED,
	    PAM_ACCT_MGMT, "pam_acct_mgmt");

	if (retval != PAM_SUCCESS &&
	    retval != PAM_NEW_AUTHTOK_REQD) {
		(void) pam_set_item(pamh, PAM_AUTHTOK, NULL);
	}
	return (retval);
}