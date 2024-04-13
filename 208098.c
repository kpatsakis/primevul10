free_pam_conf_info(pam_handle_t *pamh)
{
	pamtab_t *pamentp;
	pamtab_t *pament_trail;
	int i = pamh->include_depth;
	int j;

	for (j = 0; j < PAM_NUM_MODULE_TYPES; j++) {
		pamentp = pamh->pam_conf_info[i][j];
		pamh->pam_conf_info[i][j] = NULL;
		pament_trail = pamentp;
		while (pamentp) {
			pamentp = pamentp->next;
			free_pamconf(pament_trail);
			pament_trail = pamentp;
		}
	}
	if (pamh->pam_conf_name[i] != NULL) {
		free(pamh->pam_conf_name[i]);
		pamh->pam_conf_name[i] = NULL;
	}
}