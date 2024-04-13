R_API int r_sys_run(const ut8 *buf, int len) {
	const int sz = 4096;
	int pdelta, ret, (*cb)();
#if USE_FORK
	int st, pid;
#endif
// TODO: define R_SYS_ALIGN_FORWARD in r_util.h
	ut8 *ptr, *p = malloc ((sz + len) << 1);
	ptr = p;
	pdelta = ((size_t)(p)) & (4096 - 1);
	if (pdelta) {
		ptr += (4096 - pdelta);
	}
	if (!ptr || !buf) {
		eprintf ("r_sys_run: Cannot run empty buffer\n");
		free (p);
		return false;
	}
	memcpy (ptr, buf, len);
	r_mem_protect (ptr, sz, "rx");
	//r_mem_protect (ptr, sz, "rwx"); // try, ignore if fail
	cb = (int (*)())ptr;
#if USE_FORK
#if __UNIX__
	pid = r_sys_fork ();
#else
	pid = -1;
#endif
	if (pid < 0) {
		return cb ();
	}
	if (!pid) {
		ret = cb ();
		exit (ret);
		return ret;
	}
	st = 0;
	waitpid (pid, &st, 0);
	if (WIFSIGNALED (st)) {
		int num = WTERMSIG(st);
		eprintf ("Got signal %d\n", num);
		ret = num;
	} else {
		ret = WEXITSTATUS (st);
	}
#else
	ret = (*cb) ();
#endif
	free (p);
	return ret;
}