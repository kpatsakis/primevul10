open_pam_conf(struct pam_fh **pam_fh, pam_handle_t *pamh, char *config)
{
	struct stat64	stb;
	int		fd;

	if ((fd = open(config, O_RDONLY)) == -1) {
		__pam_log(LOG_AUTH | LOG_ALERT,
		    "open_pam_conf[%d:%s]: open(%s) failed: %s",
		    pamh->include_depth, pam_trace_cname(pamh), config,
		    strerror(errno));
		return (0);
	}
	/* Check the ownership and file modes */
	if (fstat64(fd, &stb) < 0) {
		__pam_log(LOG_AUTH | LOG_ALERT,
		    "open_pam_conf[%d:%s]: stat(%s) failed: %s",
		    pamh->include_depth, pam_trace_cname(pamh), config,
		    strerror(errno));
		(void) close(fd);
		return (0);
	}
	if (stb.st_uid != (uid_t)0) {
		__pam_log(LOG_AUTH | LOG_ALERT,
		    "open_pam_conf[%d:%s]: Owner of %s is not root",
		    pamh->include_depth, pam_trace_cname(pamh), config);
		(void) close(fd);
		return (0);
	}
	if (stb.st_mode & S_IWGRP) {
		__pam_log(LOG_AUTH | LOG_ALERT,
		    "open_pam_conf[%d:%s]: %s writable by group",
		    pamh->include_depth, pam_trace_cname(pamh), config);
		(void) close(fd);
		return (0);
	}
	if (stb.st_mode & S_IWOTH) {
		__pam_log(LOG_AUTH | LOG_ALERT,
		    "open_pam_conf[%d:%s]: %s writable by world",
		    pamh->include_depth, pam_trace_cname(pamh), config);
		(void) close(fd);
		return (0);
	}
	if ((*pam_fh = calloc(1, sizeof (struct pam_fh))) == NULL) {
		(void) close(fd);
		return (0);
	}
	(*pam_fh)->fconfig = fd;
	(*pam_fh)->bufsize = (size_t)stb.st_size;
	if (((*pam_fh)->data = mmap(0, (*pam_fh)->bufsize, PROT_READ,
	    MAP_PRIVATE, (*pam_fh)->fconfig, 0)) == MAP_FAILED) {
		(void) close(fd);
		free (*pam_fh);
		return (0);
	}
	(*pam_fh)->bufferp = (*pam_fh)->data;

	return (1);
}