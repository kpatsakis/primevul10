pam_getenvlist(pam_handle_t *pamh)
{
	int		error = PAM_SYSTEM_ERR;
	char		**list = 0;
	int		length = 0;
	env_list	*traverse;
	char		*tenv;
	size_t		tenv_size;

	pam_trace(PAM_DEBUG_DEFAULT,
	    "pam_getenvlist(%p)", (void *)pamh);

	if (pamh == NULL)
		goto out;

	/* find out how many environment variables we have */
	traverse = pamh->pam_env;
	while (traverse) {
		length++;
		traverse = traverse->next;
	}

	/* allocate the array we will return to the caller */
	if ((list = calloc(length + 1, sizeof (char *))) == NULL) {
		error = PAM_BUF_ERR;
		goto out;
	}

	/* add the variables one by one */
	length = 0;
	traverse = pamh->pam_env;
	while (traverse != NULL) {
		tenv_size = strlen(traverse->name) +
		    strlen(traverse->value) + 2; /* name=val\0 */
		if ((tenv = malloc(tenv_size)) == NULL) {
			error = PAM_BUF_ERR;
			goto out;
		}
		/*LINTED*/
		(void) sprintf(tenv, "%s=%s", traverse->name, traverse->value);
		list[length++] = tenv;
		traverse = traverse->next;
	}
	list[length] = NULL;

	error = PAM_SUCCESS;
out:
	if (error != PAM_SUCCESS) {
		/* free the partially constructed list */
		if (list) {
			length = 0;
			while (list[length] != NULL) {
				free(list[length]);
				length++;
			}
			free(list);
		}
	}
	return (error ? NULL : list);
}