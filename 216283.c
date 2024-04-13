R_API char *r_sys_getdir(void) {
#if __WINDOWS__
	return _getcwd (NULL, 0);
#else
	return getcwd (NULL, 0);
#endif
}