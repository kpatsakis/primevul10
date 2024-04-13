R_API int r_sys_thp_mode(void) {
#if __linux__
	const char *thp = "/sys/kernel/mm/transparent_hugepage/enabled";
	int ret = 0;
	char *val = r_file_slurp (thp, NULL);
	if (val) {
		if (strstr (val, "[madvise]")) {
			ret = 1;
		} else if (strstr (val, "[always]")) {
			ret = 2;
		}
		free (val);
	}

	return ret;
#else
  return 0;
#endif
}