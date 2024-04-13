obtain_authtok(pam_handle_t *pamh)
{
    char *resp;
    const void *item;
    int retval;

    retval = pam_prompt(pamh, PAM_PROMPT_ECHO_OFF, &resp, _("Password: "));

    if (retval != PAM_SUCCESS)
	return retval;

    if (resp == NULL)
	return PAM_CONV_ERR;

    /* set the auth token */
    retval = pam_set_item(pamh, PAM_AUTHTOK, resp);

    /* clean it up */
    _pam_overwrite(resp);
    _pam_drop(resp);

    if ( (retval != PAM_SUCCESS) ||
	 (retval = pam_get_item(pamh, PAM_AUTHTOK, &item))
	 != PAM_SUCCESS ) {
	return retval;
    }

    return retval;
}