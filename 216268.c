R_API bool r_sys_aslr(int val) {
	bool ret = true;
#if __linux__
	const char *rva = "/proc/sys/kernel/randomize_va_space";
	char buf[3] = {0};
	snprintf(buf, sizeof (buf), "%d\n", val != 0 ? 2 : 0);
	int fd = r_sandbox_open (rva, O_WRONLY, 0644);
	if (fd != -1) {
		if (r_sandbox_write (fd, (ut8 *)buf, sizeof (buf)) != sizeof (buf)) {
			eprintf ("Failed to set RVA\n");
			ret = false;
		}
		close (fd);
	}
#elif __FreeBSD__ && __FreeBSD_version >= 1300000
	size_t vlen = sizeof (val);
	if (sysctlbyname ("kern.elf32.aslr.enable", NULL, 0, &val, vlen) == -1) {
		eprintf ("Failed to set RVA 32 bits\n");
		return false;
	}

#if __LP64__
	if (sysctlbyname ("kern.elf64.aslr.enable", NULL, 0, &val, vlen) == -1) {
		eprintf ("Failed to set RVA 64 bits\n");
		ret = false;
	}
#endif
#endif
	return ret;
}