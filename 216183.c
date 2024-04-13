R_API int r_sys_setenv(const char *key, const char *value) {
	if (!key) {
		return 0;
	}
#if __UNIX__
	if (!value) {
		unsetenv (key);
		return 0;
	}
	return setenv (key, value, 1);
#elif __WINDOWS__
	LPTSTR key_ = r_sys_conv_utf8_to_win (key);
	LPTSTR value_ = r_sys_conv_utf8_to_win (value);
	int ret = SetEnvironmentVariable (key_, value_);
	if (!ret) {
		r_sys_perror ("r_sys_setenv/SetEnvironmentVariable");
	}
	free (key_);
	free (value_);
	return ret ? 0 : -1;
#else
#warning r_sys_setenv : unimplemented for this platform
	return 0;
#endif
}