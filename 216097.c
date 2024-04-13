R_API void r_sys_exit(int status, bool nocleanup) {
	if (nocleanup) {
		_exit (status);
	} else {
		exit (status);
	}
}