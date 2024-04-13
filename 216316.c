R_API char *r_sys_pid_to_path(int pid) {
#if __WINDOWS__
	// TODO: add maximum path length support
	HANDLE processHandle;
	const DWORD maxlength = MAX_PATH;
	TCHAR filename[MAX_PATH];
	char *result = NULL;

	processHandle = OpenProcess (PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!processHandle) {
		eprintf ("r_sys_pid_to_path: Cannot open process.\n");
		return NULL;
	}
	DWORD length = GetModuleFileNameEx (processHandle, NULL, filename, maxlength);
	if (length == 0) {
		// Upon failure fallback to GetProcessImageFileName
		length = GetProcessImageFileName (processHandle, filename, maxlength);
		CloseHandle (processHandle);
		if (length == 0) {
			eprintf ("r_sys_pid_to_path: Error calling GetProcessImageFileName\n");
			return NULL;
		}
		// Convert NT path to win32 path
		char *name = r_sys_conv_win_to_utf8 (filename);
		if (!name) {
			eprintf ("r_sys_pid_to_path: Error converting to utf8\n");
			return NULL;
		}
		char *tmp = strchr (name + 1, '\\');
		if (!tmp) {
			free (name);
			eprintf ("r_sys_pid_to_path: Malformed NT path\n");
			return NULL;
		}
		tmp = strchr (tmp + 1, '\\');
		if (!tmp) {
			free (name);
			eprintf ("r_sys_pid_to_path: Malformed NT path\n");
			return NULL;
		}
		length = tmp - name;
		tmp = malloc (length + 1);
		if (!tmp) {
			free (name);
			eprintf ("r_sys_pid_to_path: Error allocating memory\n");
			return NULL;
		}
		strncpy (tmp, name, length);
		tmp[length] = '\0';
		TCHAR device[MAX_PATH];
		for (TCHAR drv[] = TEXT("A:"); drv[0] <= TEXT('Z'); drv[0]++) {
			if (QueryDosDevice (drv, device, maxlength) > 0) {
				char *dvc = r_sys_conv_win_to_utf8 (device);
				if (!dvc) {
					free (name);
					free (tmp);
					eprintf ("r_sys_pid_to_path: Error converting to utf8\n");
					return NULL;
				}
				if (!strcmp (tmp, dvc)) {
					free (tmp);
					free (dvc);
					char *d = r_sys_conv_win_to_utf8 (drv);
					if (!d) {
						free (name);
						eprintf ("r_sys_pid_to_path: Error converting to utf8\n");
						return NULL;
					}
					tmp = r_str_newf ("%s%s", d, &name[length]);
					free (d);
					if (!tmp) {
						free (name);
						eprintf ("r_sys_pid_to_path: Error calling r_str_newf\n");
						return NULL;
					}
					result = strdup (tmp);
					break;
				}
				free (dvc);
			}
		}
		free (name);
		free (tmp);
	} else {
		CloseHandle (processHandle);
		result = r_sys_conv_win_to_utf8 (filename);
	}
	return result;
#elif __APPLE__
#if __POWERPC__
#warning TODO getpidproc
	return NULL;
#else
	char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
	pathbuf[0] = 0;
	int ret = proc_pidpath (pid, pathbuf, sizeof (pathbuf));
	if (ret <= 0) {
		return NULL;
	}
	return strdup (pathbuf);
#endif
#else
	int ret;
#if __FreeBSD__
	char pathbuf[PATH_MAX];
	size_t pathbufl = sizeof (pathbuf);
	int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, pid};
	ret = sysctl (mib, 4, pathbuf, &pathbufl, NULL, 0);
	if (ret != 0) {
		return NULL;
	}
#else
	char buf[128], pathbuf[1024];
	snprintf (buf, sizeof (buf), "/proc/%d/exe", pid);
	ret = readlink (buf, pathbuf, sizeof (pathbuf)-1);
	if (ret < 1) {
		return NULL;
	}
	pathbuf[ret] = 0;
#endif
	return strdup (pathbuf);
#endif
}