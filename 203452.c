static bool mtk3301_rate_switcher(struct gps_device_t *session, double rate)
{
    char buf[78];

    /*@i1@*/ unsigned int milliseconds = 1000 * rate;
    if (rate > 1)
	milliseconds = 1000;
    else if (rate < 0.2)
	milliseconds = 200;

    (void)snprintf(buf, sizeof(buf), "$PMTK300,%u,0,0,0,0", milliseconds);
    (void)nmea_send(session, buf);	/* Fix interval */
    return true;
}