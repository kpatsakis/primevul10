pam_getenv(pam_handle_t *pamh, const char *name)
{
	int		error = PAM_SYSTEM_ERR;
	env_list	*traverse;

	pam_trace(PAM_DEBUG_DEFAULT,
	    "pam_getenv(%p, %p)", (void *)pamh, (void *)name);

	if (pamh == NULL || name == NULL)
		goto out;

	/* check to see if we already have this variable in the PAM handle */
	traverse = pamh->pam_env;
	while (traverse && strncmp(traverse->name, name, strlen(name))) {
		traverse = traverse->next;
	}
	error = (traverse ? PAM_SUCCESS : PAM_SYSTEM_ERR);
	pam_trace(PAM_DEBUG_DEFAULT,
	    "pam_getenv(%p, %s)=%s", (void *)pamh, name,
	    traverse ? traverse->value : "NULL");
out:
	return (error ? NULL : strdup(traverse->value));
}