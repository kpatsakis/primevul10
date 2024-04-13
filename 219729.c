pam_sm_authenticate(pam_handle_t *pamh, int flags UNUSED,
		    int argc, const char **argv)
{
     const char *username;
     const void *password;
     const char *database = NULL;
     const char *cryptmode = NULL;
     int retval = PAM_AUTH_ERR, ctrl;

     /* parse arguments */
     ctrl = _pam_parse(pamh, argc, argv, &database, &cryptmode);
     if (database == NULL) {
        pam_syslog(pamh, LOG_ERR, "can not get the database name");
        return PAM_SERVICE_ERR;
     }

     /* Get the username */
     retval = pam_get_user(pamh, &username, NULL);
     if ((retval != PAM_SUCCESS) || (!username)) {
        pam_syslog(pamh, LOG_ERR, "can not get the username");
        return PAM_SERVICE_ERR;
     }

     if ((ctrl & PAM_USE_FPASS_ARG) == 0 && (ctrl & PAM_TRY_FPASS_ARG) == 0) {
        /* Converse to obtain a password */
        retval = obtain_authtok(pamh);
        if (retval != PAM_SUCCESS) {
	    pam_syslog(pamh, LOG_ERR, "can not obtain password from user");
	    return retval;
        }
     }

     /* Check if we got a password */
     retval = pam_get_item(pamh, PAM_AUTHTOK, &password);
     if (retval != PAM_SUCCESS || password == NULL) {
        if ((ctrl & PAM_TRY_FPASS_ARG) != 0) {
	    /* Converse to obtain a password */
	    retval = obtain_authtok(pamh);
	    if (retval != PAM_SUCCESS) {
	        pam_syslog(pamh, LOG_ERR, "can not obtain password from user");
		return retval;
	    }
	    retval = pam_get_item(pamh, PAM_AUTHTOK, &password);
	}
	if (retval != PAM_SUCCESS || password == NULL) {
	    pam_syslog(pamh, LOG_ERR, "can not recover user password");
	    return PAM_AUTHTOK_RECOVERY_ERR;
	}
     }

     if (ctrl & PAM_DEBUG_ARG)
	 pam_syslog(pamh, LOG_INFO, "Verify user `%s' with a password",
		    username);

     /* Now use the username to look up password in the database file */
     retval = user_lookup(pamh, database, cryptmode, username, password, ctrl);
     switch (retval) {
	 case -2:
	     /* some sort of system error. The log was already printed */
	     return PAM_SERVICE_ERR;
	 case -1:
	     /* incorrect password */
	     pam_syslog(pamh, LOG_WARNING,
			"user `%s' denied access (incorrect password)",
			username);
	     return PAM_AUTH_ERR;
	 case 1:
	     /* the user does not exist in the database */
	     if (ctrl & PAM_DEBUG_ARG)
		 pam_syslog(pamh, LOG_NOTICE,
			    "user `%s' not found in the database", username);
	     return PAM_USER_UNKNOWN;
	 case 0:
	     /* Otherwise, the authentication looked good */
	     pam_syslog(pamh, LOG_NOTICE, "user '%s' granted access", username);
	     return PAM_SUCCESS;
	 default:
	     /* we don't know anything about this return value */
	     pam_syslog(pamh, LOG_ERR,
		      "internal module error (retval = %d, user = `%s'",
		      retval, username);
	     return PAM_SERVICE_ERR;
     }

     /* should not be reached */
     return PAM_IGNORE;
}