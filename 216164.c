R_API char *r_str_r2_prefix(const char *str) {
	return r_str_newf ("%s%s%s", r_sys_prefix (NULL), R_SYS_DIR, str);
}