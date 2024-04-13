R_API bool r_sys_arch_match(const char *archstr, const char *arch) {
	char *ptr;
	if (!archstr || !arch || !*archstr || !*arch) {
		return true;
	}
	if (!strcmp (archstr, "*") || !strcmp (archstr, "any")) {
		return true;
	}
	if (!strcmp (archstr, arch)) {
		return true;
	}
	if ((ptr = strstr (archstr, arch))) {
		char p = ptr[strlen (arch)];
		if (!p || p==',') {
			return true;
		}
	}
	return false;
}