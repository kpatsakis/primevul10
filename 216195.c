R_API int r_sys_crash_handler(const char *cmd) {
#ifndef __WINDOWS__
	int sig[] = { SIGINT, SIGSEGV, SIGBUS, SIGQUIT, SIGHUP, 0 };

	if (!checkcmd (cmd)) {
		return false;
	}
#ifdef HAVE_BACKTRACE
	void *array[1];
	/* call this outside of the signal handler to init it safely */
	backtrace (array, 1);
#endif

	free (crash_handler_cmd);
	crash_handler_cmd = strdup (cmd);

	r_sys_sigaction (sig, signal_handler);
#else
#pragma message ("r_sys_crash_handler : unimplemented for this platform")
#endif
	return true;
}