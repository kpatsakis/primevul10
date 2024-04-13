pam_strerror(pam_handle_t *pamh, int errnum)
{
	switch (errnum) {
	case PAM_SUCCESS:
		return (dgettext(TEXT_DOMAIN, "Success"));
	case PAM_OPEN_ERR:
		return (dgettext(TEXT_DOMAIN, "Dlopen failure"));
	case PAM_SYMBOL_ERR:
		return (dgettext(TEXT_DOMAIN, "Symbol not found"));
	case PAM_SERVICE_ERR:
		return (dgettext(TEXT_DOMAIN,
		    "Error in underlying service module"));
	case PAM_SYSTEM_ERR:
		return (dgettext(TEXT_DOMAIN, "System error"));
	case PAM_BUF_ERR:
		return (dgettext(TEXT_DOMAIN, "Memory buffer error"));
	case PAM_CONV_ERR:
		return (dgettext(TEXT_DOMAIN, "Conversation failure"));
	case PAM_PERM_DENIED:
		return (dgettext(TEXT_DOMAIN, "Permission denied"));
	case PAM_MAXTRIES:
		return (dgettext(TEXT_DOMAIN,
		    "Maximum number of attempts exceeded"));
	case PAM_AUTH_ERR:
		return (dgettext(TEXT_DOMAIN, "Authentication failed"));
	case PAM_NEW_AUTHTOK_REQD:
		return (dgettext(TEXT_DOMAIN, "Get new authentication token"));
	case PAM_CRED_INSUFFICIENT:
		return (dgettext(TEXT_DOMAIN, "Insufficient credentials"));
	case PAM_AUTHINFO_UNAVAIL:
		return (dgettext(TEXT_DOMAIN,
		    "Can not retrieve authentication info"));
	case PAM_USER_UNKNOWN:
		return (dgettext(TEXT_DOMAIN, "No account present for user"));
	case PAM_CRED_UNAVAIL:
		return (dgettext(TEXT_DOMAIN,
		    "Can not retrieve user credentials"));
	case PAM_CRED_EXPIRED:
		return (dgettext(TEXT_DOMAIN,
		    "User credentials have expired"));
	case PAM_CRED_ERR:
		return (dgettext(TEXT_DOMAIN,
		    "Failure setting user credentials"));
	case PAM_ACCT_EXPIRED:
		return (dgettext(TEXT_DOMAIN, "User account has expired"));
	case PAM_AUTHTOK_EXPIRED:
		return (dgettext(TEXT_DOMAIN, "User password has expired"));
	case PAM_SESSION_ERR:
		return (dgettext(TEXT_DOMAIN,
		    "Can not make/remove entry for session"));
	case PAM_AUTHTOK_ERR:
		return (dgettext(TEXT_DOMAIN,
		    "Authentication token manipulation error"));
	case PAM_AUTHTOK_RECOVERY_ERR:
		return (dgettext(TEXT_DOMAIN,
		    "Authentication token can not be recovered"));
	case PAM_AUTHTOK_LOCK_BUSY:
		return (dgettext(TEXT_DOMAIN,
		    "Authentication token lock busy"));
	case PAM_AUTHTOK_DISABLE_AGING:
		return (dgettext(TEXT_DOMAIN,
		    "Authentication token aging disabled"));
	case PAM_NO_MODULE_DATA:
		return (dgettext(TEXT_DOMAIN,
		    "Module specific data not found"));
	case PAM_IGNORE:
		return (dgettext(TEXT_DOMAIN, "Ignore module"));
	case PAM_ABORT:
		return (dgettext(TEXT_DOMAIN, "General PAM failure "));
	case PAM_TRY_AGAIN:
		return (dgettext(TEXT_DOMAIN,
		    "Unable to complete operation. Try again"));
	default:
		return (dgettext(TEXT_DOMAIN, "Unknown error"));
	}
}