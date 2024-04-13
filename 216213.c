R_API int r_str_rwx(const char *str) {
	int ret = atoi (str);
	if (!ret) {
		ret |= strchr (str, 'm') ? 16 : 0;
		ret |= strchr (str, 'r') ? 4 : 0;
		ret |= strchr (str, 'w') ? 2 : 0;
		ret |= strchr (str, 'x') ? 1 : 0;
	} else if (ret < 0 || ret >= R_ARRAY_SIZE (rwxstr)) {
		ret = 0;
	}
	return ret;
}