pam_get_data(const pam_handle_t *pamh, const char *module_data_name,
    const void **data)
{
	struct pam_module_data *psd;

	if (pamh == NULL || (pamh->pam_inmodule != WO_OK) ||
	    module_data_name == NULL) {
		pam_trace(PAM_DEBUG_DATA,
		    "pam_get_data(%p:%s:%d)=%p", (void *)pamh,
		    module_data_name ? module_data_name : "NULL",
		    pamh->pam_inmodule, *data);
		return (PAM_SYSTEM_ERR);
	}

	for (psd = pamh->ssd; psd; psd = psd->next) {
		if (strcmp(psd->module_data_name, module_data_name) == 0) {
			*data = psd->data;
			pam_trace(PAM_DEBUG_DATA,
			    "pam_get_data(%p:%s)=%p", (void *)pamh,
			    module_data_name, *data);
			return (PAM_SUCCESS);
		}
	}
	pam_trace(PAM_DEBUG_DATA,
	    "pam_get_data(%p:%s)=%s", (void *)pamh, module_data_name,
	    "PAM_NO_MODULE_DATA");

	return (PAM_NO_MODULE_DATA);
}