R_API int r_sys_cmdbg (const char *str) {
#if __UNIX__
	int ret, pid = r_sys_fork ();
	if (pid == -1) {
		return -1;
	}
	if (pid) {
		return pid;
	}
	ret = r_sandbox_system (str, 0);
	eprintf ("{exit: %d, pid: %d, cmd: \"%s\"}", ret, pid, str);
	exit (0);
	return -1;
#else
#ifdef _MSC_VER
#pragma message ("r_sys_cmdbg is not implemented for this platform")
#else
#warning r_sys_cmdbg is not implemented for this platform
#endif
	return -1;
#endif
}