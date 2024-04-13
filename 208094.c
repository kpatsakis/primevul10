sm_name(int ind)
{
	switch (ind) {
	case PAM_AUTHENTICATE:
		return (PAM_SM_AUTHENTICATE);
	case PAM_SETCRED:
		return (PAM_SM_SETCRED);
	case PAM_ACCT_MGMT:
		return (PAM_SM_ACCT_MGMT);
	case PAM_OPEN_SESSION:
		return (PAM_SM_OPEN_SESSION);
	case PAM_CLOSE_SESSION:
		return (PAM_SM_CLOSE_SESSION);
	case PAM_CHAUTHTOK:
		return (PAM_SM_CHAUTHTOK);
	}
	return (NULL);
}