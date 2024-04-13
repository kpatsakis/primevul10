pam_get_item(const pam_handle_t *pamh, int item_type, void **item)
{
	struct pam_item *pip;
	char	iname_buf[PAM_MAX_MSG_SIZE];

	if (((pam_debug & PAM_DEBUG_ITEM) == 0) || (pamh == NULL)) {
		pam_trace(PAM_DEBUG_ITEM, "pam_get_item(%p:%s)",
		    (void *)pamh, pam_trace_iname(item_type, iname_buf));
	}

	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);

	if (item_type <= 0 || item_type >= PAM_MAX_ITEMS)
		return (PAM_SYMBOL_ERR);

	if ((pamh->pam_inmodule != WO_OK) &&
	    ((item_type == PAM_AUTHTOK || item_type == PAM_OLDAUTHTOK))) {
		__pam_log(LOG_AUTH | LOG_NOTICE, "pam_get_item(%s) called from "
		    "a non module context",
		    pam_trace_iname(item_type, iname_buf));
		return (PAM_PERM_DENIED);
	}

	pip = (struct pam_item *)&(pamh->ps_item[item_type]);

	*item = pip->pi_addr;
	switch (item_type) {
	case PAM_CONV:
		pam_trace(PAM_DEBUG_ITEM, "pam_get_item(%p:%s)=%p",
		    (void *)pamh,
		    pam_trace_iname(item_type, iname_buf),
		    (void *)((struct pam_conv *)*item)->conv);
		break;
	case PAM_REPOSITORY:
		pam_trace(PAM_DEBUG_ITEM, "pam_get_item(%p:%s)=%s",
		    (void *)pamh,
		    pam_trace_iname(item_type, iname_buf),
		    *item ? (((struct pam_repository *)*item)->type ?
		    ((struct pam_repository *)*item)->type : "NULL") :
		    "NULL");
		break;
	case PAM_AUTHTOK:
	case PAM_OLDAUTHTOK:
#ifdef	DEBUG
		if (pam_debug & PAM_DEBUG_AUTHTOK)
			pam_trace(PAM_DEBUG_ITEM,
			    "pam_get_item(%p:%s)=%s", (void *)pamh,
			    pam_trace_iname(item_type, iname_buf),
			    *item ? *(char **)item : "NULL");
		else
#endif	/* DEBUG */
			pam_trace(PAM_DEBUG_ITEM,
			    "pam_get_item(%p:%s)=%s", (void *)pamh,
			    pam_trace_iname(item_type, iname_buf),
			    *item ? "********" : "NULL");
		break;
	default:
		pam_trace(PAM_DEBUG_ITEM, "pam_get_item(%p:%s)=%s",
		    (void *)pamh,
		    pam_trace_iname(item_type, iname_buf),
		    *item ? *(char **)item : "NULL");
	}

	return (PAM_SUCCESS);
}