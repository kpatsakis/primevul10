R_API int r_sys_fork() {
#if HAVE_FORK
#if __WINDOWS__
	return -1;
#else
	return fork ();
#endif
#else
	return -1;
#endif
}