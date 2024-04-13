verify_pam_conf(pamtab_t *pam, char *service)
{
	return ((pam->pam_service == (char *)NULL) ||
	    ((strcasecmp(pam->pam_service, service) == 0) &&
	    ((pam->pam_type == -1) ||
	    (pam->pam_flag == 0) ||
	    (pam->pam_err != PAM_SUCCESS) ||
	    (pam->module_path == (char *)NULL))));
}