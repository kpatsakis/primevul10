pam_end(pam_handle_t *pamh, int pam_status)
{
	struct pam_module_data *psd, *p;
	fd_list *expired;
	fd_list *traverse;
	env_list *env_expired;
	env_list *env_traverse;

	pam_trace(PAM_DEBUG_DEFAULT,
	    "pam_end(%p): status = %s", (void *)pamh,
	    pam_strerror(pamh, pam_status));

	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);

	/* call the cleanup routines for module specific data */

	psd = pamh->ssd;
	while (psd) {
		if (psd->cleanup) {
			psd->cleanup(pamh, psd->data, pam_status);
		}
		p = psd;
		psd = p->next;
		free(p->module_data_name);
		free(p);
	}
	pamh->ssd = NULL;

	/* dlclose all module fds */
	traverse = pamh->fd;
	while (traverse) {
		expired = traverse;
		traverse = traverse->next;
		(void) dlclose(expired->mh);
		free(expired);
	}
	pamh->fd = 0;

	/* remove all environment variables */
	env_traverse = pamh->pam_env;
	while (env_traverse) {
		env_expired = env_traverse;
		env_traverse = env_traverse->next;
		free_env(env_expired);
	}

	clean_up(pamh);
	return (PAM_SUCCESS);
}