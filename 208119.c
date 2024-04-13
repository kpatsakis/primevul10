(*func(pamtab_t *modulep, int ind))()
{
	void	*funcp;

	if ((funcp = modulep->function_ptr) == NULL)
		return (NULL);

	switch (ind) {
	case PAM_AUTHENTICATE:
		return (((struct auth_module *)funcp)->pam_sm_authenticate);
	case PAM_SETCRED:
		return (((struct auth_module *)funcp)->pam_sm_setcred);
	case PAM_ACCT_MGMT:
		return (((struct account_module *)funcp)->pam_sm_acct_mgmt);
	case PAM_OPEN_SESSION:
		return (((struct session_module *)funcp)->pam_sm_open_session);
	case PAM_CLOSE_SESSION:
		return (((struct session_module *)funcp)->pam_sm_close_session);
	case PAM_CHAUTHTOK:
		return (((struct password_module *)funcp)->pam_sm_chauthtok);
	}
	return (NULL);
}