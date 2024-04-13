pam_set_data(pam_handle_t *pamh, const char *module_data_name, void *data,
    void (*cleanup)(pam_handle_t *pamh, void *data, int pam_end_status))
{
	struct pam_module_data *psd;

	pam_trace(PAM_DEBUG_DATA,
	    "pam_set_data(%p:%s:%d)=%p", (void *)pamh,
	    (module_data_name != NULL) ? module_data_name : "NULL",
	    (pamh != NULL) ? pamh->pam_inmodule : -1, data);
	if (pamh == NULL || (pamh->pam_inmodule != WO_OK) ||
	    module_data_name == NULL) {
		return (PAM_SYSTEM_ERR);
	}

	/* check if module data already exists */

	for (psd = pamh->ssd; psd; psd = psd->next) {
		if (strcmp(psd->module_data_name, module_data_name) == 0) {
			/* clean up original data before setting the new data */
			if (psd->cleanup) {
				psd->cleanup(pamh, psd->data, PAM_SUCCESS);
			}
			psd->data = (void *)data;
			psd->cleanup = cleanup;
			return (PAM_SUCCESS);
		}
	}

	psd = malloc(sizeof (struct pam_module_data));
	if (psd == NULL)
		return (PAM_BUF_ERR);

	psd->module_data_name = strdup(module_data_name);
	if (psd->module_data_name == NULL) {
		free(psd);
		return (PAM_BUF_ERR);
	}

	psd->data = (void *)data;
	psd->cleanup = cleanup;
	psd->next = pamh->ssd;
	pamh->ssd = psd;
	return (PAM_SUCCESS);
}