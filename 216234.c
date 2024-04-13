R_API int r_sys_truncate(const char *file, int sz) {
#if __WINDOWS__
	int fd = r_sandbox_open (file, O_RDWR, 0644);
	if (fd == -1) {
		return false;
	}
#ifdef _MSC_VER
	int r = _chsize (fd, sz);
#else
	int r = ftruncate (fd, sz);
#endif
	if (r != 0) {
		eprintf ("Could not resize '%s' file\n", file);
		close (fd);
		return false;
	}
	close (fd);
	return true;
#else
	if (r_sandbox_enable (0)) {
		return false;
	}
	return truncate (file, sz) == 0;
#endif
}