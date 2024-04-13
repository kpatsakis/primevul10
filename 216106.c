R_API int r_sys_sigaction(int *sig, void (*handler) (int)) {
	int ret, i;

	if (!sig) {
		return -EINVAL;
	}

	for (i = 0; sig[i] != 0; i++) {
		ret = signal (sig[i], handler);
		if (ret == SIG_ERR) {
			eprintf ("Failed to set signal handler for signal '%d': %s\n", sig[i], strerror(errno));
			return -1;
		}
	}
	return 0;
}