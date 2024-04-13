clean_up(pam_handle_t *pamh)
{
	int i;
	pam_repository_t *auth_rep;

	if (pamh) {
		while (pamh->include_depth >= 0) {
			free_pam_conf_info(pamh);
			pamh->include_depth--;
		}

		/* Cleanup PAM_REPOSITORY structure */
		auth_rep = pamh->ps_item[PAM_REPOSITORY].pi_addr;
		if (auth_rep != NULL) {
			if (auth_rep->type != NULL)
				free(auth_rep->type);
			if (auth_rep->scope != NULL)
				free(auth_rep->scope);
		}

		for (i = 0; i < PAM_MAX_ITEMS; i++) {
			if (pamh->ps_item[i].pi_addr != NULL) {
				if (i == PAM_AUTHTOK || i == PAM_OLDAUTHTOK) {
					(void) memset(pamh->ps_item[i].pi_addr,
					    0, pamh->ps_item[i].pi_size);
				}
				free(pamh->ps_item[i].pi_addr);
			}
		}
		free(pamh);
	}
}