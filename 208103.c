load_function(void *lfd, char *name, int (**func)())
{
	char *errmsg = NULL;

	if (lfd == NULL)
		return (PAM_SYMBOL_ERR);

	*func = (int (*)())dlsym(lfd, name);
	if (*func == NULL) {
		errmsg = dlerror();
		__pam_log(LOG_AUTH | LOG_ERR, "dlsym failed %s: error %s",
		    name, errmsg != NULL ? errmsg : "Unknown error");
		return (PAM_SYMBOL_ERR);
	}

	pam_trace(PAM_DEBUG_DEFAULT,
	    "load_function: successful load of %s", name);
	return (PAM_SUCCESS);
}