R_API int r_snprintf(char *string, int len, const char *fmt, ...) {
	va_list ap;
	va_start (ap, fmt);
	int ret = vsnprintf (string, len, fmt, ap);
	string[len - 1] = 0;
	va_end (ap);
	return ret;
}