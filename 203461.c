static void garmin_nmea_event_hook(struct gps_device_t *session,
				   event_t event)
{
    if (session->context->readonly)
	return;

    if (event == event_driver_switch) {
	/* forces a reconfigure as the following packets come in */
	session->packet.counter = 0;
    }
    if (event == event_configure) {
	/*
	 * And here's that reconfigure.  It's split up like this because
	 * receivers like the Garmin GPS-10 don't handle having having a lot of
	 * probes shoved at them very well.
	 */
	switch (session->packet.counter) {
	case 0:
	    /* reset some config, AutoFix, WGS84, PPS
	     * Set the PPS pulse length to 40ms which leaves the Garmin 18-5hz
	     * with a 160ms low state.
	     * NOTE: new PPS only takes effect after next power cycle
	     */
	    (void)nmea_send(session, "$PGRMC,A,,100,,,,,,A,,1,2,1,30");
	    break;
	case 1:
	    /* once a sec, no averaging, NMEA 2.3, WAAS */
	    (void)nmea_send(session, "$PGRMC1,1,1,1,,,,2,W,N");
	    break;
	case 2:
	    /* get some more config info */
	    (void)nmea_send(session, "$PGRMC1E");
	    break;
	case 3:
	    /* turn off all output except GGA */
	    (void)nmea_send(session, "$PGRMO,,2");
	    (void)nmea_send(session, "$PGRMO,GPGGA,1");
	    break;
	case 4:
	    /* enable GPGGA, GPGSA, GPGSV, GPRMC on Garmin serial GPS */
	    (void)nmea_send(session, "$PGRMO,GPGSA,1");
	    break;
	case 5:
	    (void)nmea_send(session, "$PGRMO,GPGSV,1");
	    break;
	case 6:
	    (void)nmea_send(session, "$PGRMO,GPRMC,1");
	    break;
	case 7:
	    (void)nmea_send(session, "$PGRMO,PGRME,1");
	    break;
	}
    }
}