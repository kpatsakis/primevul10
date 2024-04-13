R_API int r_sys_getpid() {
#if __UNIX__
	return getpid ();
#elif __WINDOWS__
	return GetCurrentProcessId();
#else
#warning r_sys_getpid not implemented for this platform
	return -1;
#endif
}