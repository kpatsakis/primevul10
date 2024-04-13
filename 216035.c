R_API int r_sys_signal(int sig, void (*handler) (int)) {
	int s[2] = { sig, 0 };
	return r_sys_sigaction (s, handler);
}