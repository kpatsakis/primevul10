R_API char **r_sys_get_environ () {
#if __APPLE__ && !HAVE_ENVIRON
	env = *_NSGetEnviron();
#else
	env = environ;
#endif
	// return environ if available??
	if (!env) {
		env = r_lib_dl_sym (NULL, "environ");
	}
	return env;
}