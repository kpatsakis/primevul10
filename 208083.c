open_module(pam_handle_t *pamh, char *module_so)
{
	struct stat64	stb;
	char		*errmsg;
	void		*lfd;
	fd_list		*module_fds = 0;
	fd_list		*trail = 0;
	fd_list		*traverse = 0;

	/* Check the ownership and file modes */
	if (stat64(module_so, &stb) < 0) {
		__pam_log(LOG_AUTH | LOG_ERR,
		    "open_module[%d:%s]: stat(%s) failed: %s",
		    pamh->include_depth, pam_trace_cname(pamh), module_so,
		    strerror(errno));
		return (NULL);
	}
	if (stb.st_uid != (uid_t)0) {
		__pam_log(LOG_AUTH | LOG_ALERT,
		    "open_module[%d:%s]: Owner of the module %s is not root",
		    pamh->include_depth, pam_trace_cname(pamh), module_so);
		return (NULL);
	}
	if (stb.st_mode & S_IWGRP) {
		__pam_log(LOG_AUTH | LOG_ALERT,
		    "open_module[%d:%s]: module %s writable by group",
		    pamh->include_depth, pam_trace_cname(pamh), module_so);
		return (NULL);
	}
	if (stb.st_mode & S_IWOTH) {
		__pam_log(LOG_AUTH | LOG_ALERT,
		    "open_module[%d:%s]: module %s writable by world",
		    pamh->include_depth, pam_trace_cname(pamh), module_so);
		return (NULL);
	}

	/*
	 * Perform the dlopen()
	 */
	lfd = (void *)dlopen(module_so, RTLD_LAZY);

	if (lfd == NULL) {
		errmsg = dlerror();
		__pam_log(LOG_AUTH | LOG_ERR, "open_module[%d:%s]: %s "
		    "failed: %s", pamh->include_depth, pam_trace_cname(pamh),
		    module_so, errmsg != NULL ? errmsg : "Unknown error");
		return (NULL);
	} else {
		/* add this fd to the pam handle */
		if ((module_fds = calloc(1, sizeof (fd_list))) == 0) {
			(void) dlclose(lfd);
			lfd = 0;
			return (NULL);
		}
		module_fds->mh = lfd;

		if (pamh->fd == 0) {
			/* adding new head of list */
			pamh->fd = module_fds;
		} else {
			/* appending to end of list */
			traverse = pamh->fd;
			while (traverse) {
				trail = traverse;
				traverse = traverse->next;
			}
			trail->next = module_fds;
		}
	}

	return (lfd);
}