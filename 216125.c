R_API int r_sys_run_rop(const ut8 *buf, int len) {
#if USE_FORK
	int st;
#endif
	// TODO: define R_SYS_ALIGN_FORWARD in r_util.h
	ut8 *bufptr = malloc (len);
	if (!bufptr) {
		eprintf ("r_sys_run_rop: Cannot allocate buffer\n");
		return false;
	}

	if (!buf) {
		eprintf ("r_sys_run_rop: Cannot execute empty rop chain\n");
		free (bufptr);
		return false;
	}
	memcpy (bufptr, buf, len);
#if USE_FORK
#if __UNIX__
	pid_t pid = r_sys_fork ();
#else
	pid = -1;
#endif
	if (pid < 0) {
		R_SYS_ASM_START_ROP ();
	} else {
		R_SYS_ASM_START_ROP ();
		exit (0);
                return 0;
	}
	st = 0;
	if (waitpid (pid, &st, 0) == -1) {
            eprintf ("r_sys_run_rop: waitpid failed\n");
            free (bufptr);
            return -1;
        }
	if (WIFSIGNALED (st)) {
		int num = WTERMSIG (st);
		eprintf ("Got signal %d\n", num);
		ret = num;
	} else {
		ret = WEXITSTATUS (st);
	}
#else
	R_SYS_ASM_START_ROP ();
#endif
	free (bufptr);
	return 0;
}