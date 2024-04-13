read_pam_conf(pam_handle_t *pamh, char *config)
{
	struct pam_fh	*pam_fh;
	pamtab_t	*pamentp;
	pamtab_t	*tpament;
	char		*service;
	int		error;
	int		i = pamh->include_depth;	/* include depth */
	/*
	 * service types:
	 * error (-1), "auth" (0), "account" (1), "session" (2), "password" (3)
	 */
	int service_found[PAM_NUM_MODULE_TYPES+1] = {0, 0, 0, 0, 0};

	(void) pam_get_item(pamh, PAM_SERVICE, (void **)&service);
	if (service == NULL || *service == '\0') {
		__pam_log(LOG_AUTH | LOG_ERR, "No service name");
		return (PAM_SYSTEM_ERR);
	}

	pamh->pam_conf_name[i] = strdup(config);
	pam_trace(PAM_DEBUG_CONF, "read_pam_conf[%d:%s](%p) open(%s)",
	    i, pam_trace_cname(pamh), (void *)pamh, config);
	if (open_pam_conf(&pam_fh, pamh, config) == 0) {
		return (PAM_SYSTEM_ERR);
	}

	while ((error =
	    get_pam_conf_entry(pam_fh, pamh, &pamentp)) == PAM_SUCCESS &&
	    pamentp) {

		/* See if entry is this service and valid */
		if (verify_pam_conf(pamentp, service)) {
			pam_trace(PAM_DEBUG_CONF,
			    "read_pam_conf[%d:%s](%p): bad entry error %s",
			    i, pam_trace_cname(pamh), (void *)pamh, service);

			error = PAM_SYSTEM_ERR;
			free_pamconf(pamentp);
			goto out;
		}
		if (strcasecmp(pamentp->pam_service, service) == 0) {
			pam_trace(PAM_DEBUG_CONF,
			    "read_pam_conf[%d:%s](%p): processing %s",
			    i, pam_trace_cname(pamh), (void *)pamh, service);
			/* process first service entry */
			if (service_found[pamentp->pam_type + 1] == 0) {
				/* purge "other" entries */
				while ((tpament = pamh->pam_conf_info[i]
				    [pamentp->pam_type]) != NULL) {
					pam_trace(PAM_DEBUG_CONF,
					    "read_pam_conf(%p): purging "
					    "\"other\"[%d:%s][%s]",
					    (void *)pamh, i,
					    pam_trace_cname(pamh),
					    pam_snames[pamentp->pam_type]);
					pamh->pam_conf_info[i]
					    [pamentp->pam_type] = tpament->next;
					free_pamconf(tpament);
				}
				/* add first service entry */
				pam_trace(PAM_DEBUG_CONF,
				    "read_pam_conf(%p): adding 1st "
				    "%s[%d:%s][%s]",
				    (void *)pamh, service, i,
				    pam_trace_cname(pamh),
				    pam_snames[pamentp->pam_type]);
				pamh->pam_conf_info[i][pamentp->pam_type] =
				    pamentp;
				service_found[pamentp->pam_type + 1] = 1;
			} else {
				/* append more service entries */
				pam_trace(PAM_DEBUG_CONF,
				    "read_pam_conf(%p): adding more "
				    "%s[%d:%s][%s]",
				    (void *)pamh, service, i,
				    pam_trace_cname(pamh),
				    pam_snames[pamentp->pam_type]);
				tpament =
				    pamh->pam_conf_info[i][pamentp->pam_type];
				while (tpament->next != NULL) {
					tpament = tpament->next;
				}
				tpament->next = pamentp;
			}
		} else if (service_found[pamentp->pam_type + 1] == 0) {
			/* See if "other" entry available and valid */
			if (verify_pam_conf(pamentp, "other")) {
				pam_trace(PAM_DEBUG_CONF,
				    "read_pam_conf(%p): bad entry error %s "
				    "\"other\"[%d:%s]",
				    (void *)pamh, service, i,
				    pam_trace_cname(pamh));
				error = PAM_SYSTEM_ERR;
				free_pamconf(pamentp);
				goto out;
			}
			if (strcasecmp(pamentp->pam_service, "other") == 0) {
				pam_trace(PAM_DEBUG_CONF,
				    "read_pam_conf(%p): processing "
				    "\"other\"[%d:%s]", (void *)pamh, i,
				    pam_trace_cname(pamh));
				if ((tpament = pamh->pam_conf_info[i]
				    [pamentp->pam_type]) == NULL) {
					/* add first "other" entry */
					pam_trace(PAM_DEBUG_CONF,
					    "read_pam_conf(%p): adding 1st "
					    "other[%d:%s][%s]", (void *)pamh, i,
					    pam_trace_cname(pamh),
					    pam_snames[pamentp->pam_type]);
					pamh->pam_conf_info[i]
					    [pamentp->pam_type] = pamentp;
				} else {
					/* append more "other" entries */
					pam_trace(PAM_DEBUG_CONF,
					    "read_pam_conf(%p): adding more "
					    "other[%d:%s][%s]", (void *)pamh, i,
					    pam_trace_cname(pamh),
					    pam_snames[pamentp->pam_type]);
					while (tpament->next != NULL) {
						tpament = tpament->next;
					}
					tpament->next = pamentp;
				}
			} else {
				/* irrelevant entry */
				free_pamconf(pamentp);
			}
		} else {
			/* irrelevant entry */
			free_pamconf(pamentp);
		}
	}
out:
	(void) close_pam_conf(pam_fh);
	if (error != PAM_SUCCESS)
		free_pam_conf_info(pamh);
	return (error);
}