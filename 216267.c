R_API RSysInfo *r_sys_info(void) {
#if __UNIX__
	struct utsname un = {{0}};
	if (uname (&un) != -1) {
		RSysInfo *si = R_NEW0 (RSysInfo);
		if (si) {
			si->sysname  = strdup (un.sysname);
			si->nodename = strdup (un.nodename);
			si->release  = strdup (un.release);
			si->version  = strdup (un.version);
			si->machine  = strdup (un.machine);
			return si;
		}
	}
#elif __WINDOWS__
	HKEY key;
	DWORD type;
	DWORD size;
	DWORD major;
	DWORD minor;
	char tmp[256] = {0};
	RSysInfo *si = R_NEW0 (RSysInfo);
	if (!si) {
		return NULL;
	}
	
	if (RegOpenKeyExA (HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0,
		KEY_QUERY_VALUE, &key) != ERROR_SUCCESS) {
		r_sys_perror ("r_sys_info/RegOpenKeyExA");
		r_sys_info_free (si);
		return NULL;
	}

	size = sizeof (tmp);
	if (RegQueryValueExA (key, "ProductName", NULL, &type,
		(LPBYTE)&tmp, &size) != ERROR_SUCCESS
		|| type != REG_SZ) {
		goto beach;
	}
	si->sysname = strdup (tmp);

	size = sizeof (major);
	if (RegQueryValueExA (key, "CurrentMajorVersionNumber", NULL, &type,
		(LPBYTE)&major, &size) != ERROR_SUCCESS
		|| type != REG_DWORD) {
		goto beach;
	}
	size = sizeof (minor);
	if (RegQueryValueExA (key, "CurrentMinorVersionNumber", NULL, &type,
		(LPBYTE)&minor, &size) != ERROR_SUCCESS
		|| type != REG_DWORD) {
		goto beach;
	}

	size = sizeof (tmp);
	if (RegQueryValueExA (key, "CurrentBuild", NULL, &type,
		(LPBYTE)&tmp, &size) != ERROR_SUCCESS
		|| type != REG_SZ) {
		goto beach;
	}
	si->version = r_str_newf ("%d.%d.%s", major, minor, tmp);

	size = sizeof (tmp);
	if (RegQueryValueExA (key, "ReleaseId", NULL, &type,
		(LPBYTE)tmp, &size) != ERROR_SUCCESS
		|| type != REG_SZ) {
		goto beach;
	}
	si->release = strdup (tmp);
beach:
	RegCloseKey (key);
	return si;
#endif
	return NULL;
}