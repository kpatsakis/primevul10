R_API int r_sys_sigaction(int *sig, void (*handler) (int)) {
	struct sigaction sigact = { };
	int ret, i;

	if (!sig) {
		return -EINVAL;
	}

	sigact.sa_handler = handler;
	sigemptyset (&sigact.sa_mask);

	for (i = 0; sig[i] != 0; i++) {
		sigaddset (&sigact.sa_mask, sig[i]);
	}

	for (i = 0; sig[i] != 0; i++) {
		ret = sigaction (sig[i], &sigact, NULL);
		if (ret) {
			eprintf ("Failed to set signal handler for signal '%d': %s\n", sig[i], strerror(errno));
			return ret;
		}
	}

	return 0;
}