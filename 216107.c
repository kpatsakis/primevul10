R_API bool r_sys_getenv_asbool(const char *key) {
	char *env = r_sys_getenv (key);
	const bool res = (env && *env == '1');
	free (env);
	return res;
}