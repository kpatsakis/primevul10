R_API char *r_str_home(const char *str) {
	char *dst, *home = r_sys_getenv (R_SYS_HOME);
	size_t length;
	if (!home) {
		home = r_file_tmpdir ();
		if (!home) {
			return NULL;
		}
	}
	length = strlen (home) + 1;
	if (str) {
		length += strlen (R_SYS_DIR) + strlen (str);
	}
	dst = (char *)malloc (length);
	if (!dst) {
		goto fail;
	}
	int home_len = strlen (home);
	memcpy (dst, home, home_len + 1);
	if (str) {
		dst[home_len] = R_SYS_DIR[0];
		strcpy (dst + home_len + 1, str);
	}
fail:
	free (home);
	return dst;
}