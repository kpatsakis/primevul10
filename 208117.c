pam_start(const char *service, const char *user,
    const struct pam_conv *pam_conv, pam_handle_t **pamh)
{
	int	err;

	*pamh = calloc(1, sizeof (struct pam_handle));

	pam_settrace();
	pam_trace(PAM_DEBUG_DEFAULT,
	    "pam_start(%s,%s,%p:%p) - debug = %x",
	    service ? service : "NULL", user ? user : "NULL", (void *)pam_conv,
	    (void *)*pamh, pam_debug);

	if (*pamh == NULL)
		return (PAM_BUF_ERR);

	(*pamh)->pam_inmodule = RO_OK;		/* OK to set RO items */
	if ((err = pam_set_item(*pamh, PAM_SERVICE, (void *)service))
	    != PAM_SUCCESS) {
		clean_up(*pamh);
		*pamh = NULL;
		return (err);
	}

	if ((err = pam_set_item(*pamh, PAM_USER, (void *)user))
	    != PAM_SUCCESS) {
		clean_up(*pamh);
		*pamh = NULL;
		return (err);
	}

	if ((err = pam_set_item(*pamh, PAM_CONV, (void *)pam_conv))
	    != PAM_SUCCESS) {
		clean_up(*pamh);
		*pamh = NULL;
		return (err);
	}

	(*pamh)->pam_inmodule = RW_OK;
	return (PAM_SUCCESS);
}