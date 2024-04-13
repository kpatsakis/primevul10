void mp4box_log(const char *fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	vfprintf(stderr, fmt, vlist);
	fflush(stderr);
	va_end(vl);
}