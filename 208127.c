pam_set_item(pam_handle_t *pamh, int item_type, const void *item)
{
	struct pam_item *pip;
	int	size;
	char	iname_buf[PAM_MAX_MSG_SIZE];

	if (((pam_debug & PAM_DEBUG_ITEM) == 0) || (pamh == NULL)) {
		pam_trace(PAM_DEBUG_DEFAULT,
		    "pam_set_item(%p:%s)", (void *)pamh,
		    pam_trace_iname(item_type, iname_buf));
	}

	if (pamh == NULL)
		return (PAM_SYSTEM_ERR);

	/* check read only items */
	if ((item_type == PAM_SERVICE) && (pamh->pam_inmodule != RO_OK))
		return (PAM_PERM_DENIED);

	/*
	 * Check that item_type is within valid range
	 */

	if (item_type <= 0 || item_type >= PAM_MAX_ITEMS)
		return (PAM_SYMBOL_ERR);

	pip = &(pamh->ps_item[item_type]);

	switch (item_type) {
	case PAM_AUTHTOK:
	case PAM_OLDAUTHTOK:
		if (pip->pi_addr != NULL)
			(void) memset(pip->pi_addr, 0, pip->pi_size);
		/*FALLTHROUGH*/
	case PAM_SERVICE:
	case PAM_USER:
	case PAM_TTY:
	case PAM_RHOST:
	case PAM_RUSER:
	case PAM_USER_PROMPT:
	case PAM_RESOURCE:
	case PAM_AUSER:
		if (pip->pi_addr != NULL) {
			free(pip->pi_addr);
		}

		if (item == NULL) {
			pip->pi_addr = NULL;
			pip->pi_size = 0;
		} else {
			pip->pi_addr = strdup((char *)item);
			if (pip->pi_addr == NULL) {
				pip->pi_size = 0;
				return (PAM_BUF_ERR);
			}
			pip->pi_size = strlen(pip->pi_addr);
		}
		break;
	case PAM_CONV:
		if (pip->pi_addr != NULL)
			free(pip->pi_addr);
		size = sizeof (struct pam_conv);
		if ((pip->pi_addr = calloc(1, size)) == NULL)
			return (PAM_BUF_ERR);
		if (item != NULL)
			(void) memcpy(pip->pi_addr, item, (unsigned int) size);
		else
			(void) memset(pip->pi_addr, 0, size);
		pip->pi_size = size;
		break;
	case PAM_REPOSITORY:
		if (pip->pi_addr != NULL) {
			pam_repository_t *auth_rep;

			auth_rep = (pam_repository_t *)pip->pi_addr;
			if (auth_rep->type != NULL)
				free(auth_rep->type);
			if (auth_rep->scope != NULL)
				free(auth_rep->scope);
			free(auth_rep);
		}
		if (item != NULL) {
			pam_repository_t *s, *d;

			size = sizeof (struct pam_repository);
			pip->pi_addr = calloc(1, size);
			if (pip->pi_addr == NULL)
				return (PAM_BUF_ERR);

			s = (struct pam_repository *)item;
			d = (struct pam_repository *)pip->pi_addr;

			d->type = strdup(s->type);
			if (d->type == NULL)
				return (PAM_BUF_ERR);
			d->scope = malloc(s->scope_len);
			if (d->scope == NULL)
				return (PAM_BUF_ERR);
			(void) memcpy(d->scope, s->scope, s->scope_len);
			d->scope_len = s->scope_len;
		}
		pip->pi_size = size;
		break;
	default:
		return (PAM_SYMBOL_ERR);
	}
	switch (item_type) {
	case PAM_CONV:
		pam_trace(PAM_DEBUG_ITEM, "pam_set_item(%p:%s)=%p",
		    (void *)pamh,
		    pam_trace_iname(item_type, iname_buf),
		    item ? (void *)((struct pam_conv *)item)->conv :
		    (void *)0);
		break;
	case PAM_REPOSITORY:
		pam_trace(PAM_DEBUG_ITEM, "pam_set_item(%p:%s)=%s",
		    (void *)pamh,
		    pam_trace_iname(item_type, iname_buf),
		    item ? (((struct pam_repository *)item)->type ?
		    ((struct pam_repository *)item)->type : "NULL") :
		    "NULL");
		break;
	case PAM_AUTHTOK:
	case PAM_OLDAUTHTOK:
#ifdef	DEBUG
		if (pam_debug & PAM_DEBUG_AUTHTOK)
			pam_trace(PAM_DEBUG_ITEM,
			    "pam_set_item(%p:%s)=%s", (void *)pamh,
			    pam_trace_iname(item_type, iname_buf),
			    item ? (char *)item : "NULL");
		else
#endif	/* DEBUG */
			pam_trace(PAM_DEBUG_ITEM,
			    "pam_set_item(%p:%s)=%s", (void *)pamh,
			    pam_trace_iname(item_type, iname_buf),
			    item ? "********" : "NULL");
		break;
	default:
		pam_trace(PAM_DEBUG_ITEM, "pam_set_item(%p:%s)=%s",
		    (void *)pamh,
		    pam_trace_iname(item_type, iname_buf),
		    item ? (char *)item : "NULL");
	}

	return (PAM_SUCCESS);
}