int is_in_group(gid_t group)
{
	int ngroups_max = sysconf(_SC_NGROUPS_MAX) + 1;
	gid_t *buf = malloc(ngroups_max * sizeof(gid_t));
	int ngroups;
	int i;
	if (buf == NULL) {
		perror("malloc");
		return 0;
	}
	ngroups = getgroups(ngroups_max, buf);
	for (i = 0; i < ngroups; i++) {
		if (buf[i] == group)
			break;
	}
	free(buf);
	return i < ngroups;
}