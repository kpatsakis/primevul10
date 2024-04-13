R_API int r_sys_clearenv(void) {
#if __UNIX__
#if __APPLE__ && !HAVE_ENVIRON
	/* do nothing */
	if (!env) {
		env = r_sys_get_environ ();
		return 0;
	}
	if (env) {
		char **e = env;
		while (*e) {
			*e++ = NULL;
		}
	}
#else
	if (!environ) {
		return 0;
	}
	while (*environ) {
		*environ++ = NULL;
	}
#endif
	return 0;
#else
#ifdef _MSC_VER
#pragma message ("r_sys_clearenv : unimplemented for this platform")
#else
#warning r_sys_clearenv : unimplemented for this platform
#endif
	return 0;
#endif
}