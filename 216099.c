R_API void r_sys_perror_str(const char *fun) {
#if __UNIX__
#pragma push_macro("perror")
#undef perror
	perror (fun);
#pragma pop_macro("perror")
#elif __WINDOWS__
	LPTSTR lpMsgBuf;
	DWORD dw = GetLastError();

	if (FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL )) {
		char *err = r_sys_conv_win_to_utf8 (lpMsgBuf);
		if (err) {
			eprintf ("%s: %s\n", fun, err);
			free (err);
		}
		LocalFree (lpMsgBuf);
	} else {
		eprintf ("%s\n", fun);
	}
#endif
}