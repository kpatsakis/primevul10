__pam_log(int priority, const char *format, ...)
{
	va_list args;
	int savemask = setlogmask(LOG_MASK(priority & LOG_PRIMASK));

	va_start(args, format);
	(void) vsyslog(priority, format, args);
	va_end(args);
	(void) setlogmask(savemask);
}