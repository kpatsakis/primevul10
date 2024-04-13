static bool fury_rate_switcher(struct gps_device_t *session, double rate)
{
    char buf[78];
    double inverted;

    /* rate is a frequency, but the command takes interval in # of sedconds */
    if (rate == 0.0)
	inverted = 0.0;
    else
	inverted = 1.0/rate;
    if (inverted > 256)
	return false;
    (void)snprintf(buf, sizeof(buf), "GPS:GPGGA %d\r\n", (int)inverted);
    (void)gpsd_write(session, buf, strlen(buf));
    return true;
}