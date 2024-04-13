R_API int r_sys_chdir(const char *s) {
	return r_sandbox_chdir (s)==0;
}