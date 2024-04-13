pam_sm_acct_mgmt(pam_handle_t *pamh, int flags UNUSED,
		 int argc, const char **argv)
{
    const char *username;
    const char *database = NULL;
    const char *cryptmode = NULL;
    int retval = PAM_AUTH_ERR, ctrl;

    /* parse arguments */
    ctrl = _pam_parse(pamh, argc, argv, &database, &cryptmode);

    /* Get the username */
    retval = pam_get_user(pamh, &username, NULL);
    if ((retval != PAM_SUCCESS) || (!username)) {
        pam_syslog(pamh, LOG_ERR,"can not get the username");
        return PAM_SERVICE_ERR;
    }

    /* Now use the username to look up password in the database file */
    retval = user_lookup(pamh, database, cryptmode, username, "", ctrl);
    switch (retval) {
        case -2:
	    /* some sort of system error. The log was already printed */
	    return PAM_SERVICE_ERR;
	case -1:
	    /* incorrect password, but we don't care */
	    /* FALL THROUGH */
	case 0:
	    /* authentication succeeded. dumbest password ever. */
	    return PAM_SUCCESS;
	case 1:
	    /* the user does not exist in the database */
	    return PAM_USER_UNKNOWN;
        default:
	    /* we don't know anything about this return value */
	    pam_syslog(pamh, LOG_ERR,
		       "internal module error (retval = %d, user = `%s'",
		       retval, username);
            return PAM_SERVICE_ERR;
    }

    return PAM_SUCCESS;
}