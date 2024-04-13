static bool tnt_send(struct gps_device_t *session, const char *fmt, ...)
/* printf(3)-like TNT command generator */
{
    char buf[BUFSIZ];
    va_list ap;
    ssize_t sent;

    va_start(ap, fmt);
    (void)vsnprintf(buf, sizeof(buf) - 5, fmt, ap);
    va_end(ap);
    sent = tnt_control_send(session, buf, strlen(buf));
    if (sent == (ssize_t) strlen(buf)) {
	gpsd_report(LOG_IO, "=> GPS: %s\n", buf);
	return true;
    } else {
	gpsd_report(LOG_WARN, "=> GPS: %s FAILED\n", buf);
	return false;
    }
}