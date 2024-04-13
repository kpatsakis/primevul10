R_API void __cons_cb_fkey(RCore *core, int fkey) {
	char buf[32];
	snprintf (buf, sizeof (buf), "key.f%d", fkey);
	const char *v = r_config_get (core->config, buf);
	if (v && *v) {
		r_cons_printf ("%s\n", v);
		r_core_cmd0 (core, v);
		r_cons_flush ();
	}
}