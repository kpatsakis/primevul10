static void _pam_log(int err, CONST char *format, ...)
{
	va_list args;
	char buffer[BUFFER_SIZE];

	va_start(args, format);
	vsprintf(buffer, format, args);
	/* don't do openlog or closelog, but put our name in to be friendly */
	syslog(err, "%s: %s", pam_module_name, buffer);
	va_end(args);
}