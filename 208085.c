pam_trace(int flag, char *format, ...)
{
	va_list args;
	char message[1024];
	int savemask;

	if ((pam_debug & flag) == 0)
		return;

	savemask = setlogmask(LOG_MASK(log_priority & LOG_PRIMASK));
	(void) snprintf(message, sizeof (message), "PAM[%ld]: %s",
	    (long)getpid(), format);
	va_start(args, format);
	(void) vsyslog(log_priority, message, args);
	va_end(args);
	(void) setlogmask(savemask);
}