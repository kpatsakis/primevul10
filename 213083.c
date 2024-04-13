static int win_eprintf(const char *format, ...) {
	va_list ap;
	va_start (ap, format);
	r_cons_win_vhprintf (STD_ERROR_HANDLE, false, format, ap);
	va_end (ap);
	return 0;
}