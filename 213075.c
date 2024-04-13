static void r_core_setenv (RCore *core) {
	char *e = r_sys_getenv ("PATH");
	char *h = r_str_home (R2_HOME_BIN);
	char *n = r_str_newf ("%s%s%s", h, R_SYS_ENVSEP, e);
	r_sys_setenv ("PATH", n);
	free (n);
	free (h);
	free (e);
}