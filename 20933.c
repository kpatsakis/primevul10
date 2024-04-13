toggle_dac_capability(int writable, int enable)
{
	int rc;
	cap_t caps;
	cap_value_t capability = writable ? CAP_DAC_OVERRIDE : CAP_DAC_READ_SEARCH;

	if (getuid() != 0)
		return 0;

	caps = cap_get_proc();
	if (caps == NULL) {
		fprintf(stderr, "Unable to get current capability set: %s\n",
			strerror(errno));
		return EX_SYSERR;
	}

	if (cap_set_flag(caps, CAP_EFFECTIVE, 1, &capability,
			 enable ? CAP_SET : CAP_CLEAR) == -1) {
		fprintf(stderr, "Unable to %s effective capabilities: %s\n",
			enable ? "set" : "clear", strerror(errno));
		rc = EX_SYSERR;
		goto free_caps;
	}

	if (cap_set_proc(caps) != 0) {
		fprintf(stderr, "Unable to set current process capabilities: %s\n",
			strerror(errno));
		rc = EX_SYSERR;
	}
free_caps:
	cap_free(caps);
	return 0;
}