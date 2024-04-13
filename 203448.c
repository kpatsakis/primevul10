static void garmin_mode_switch(struct gps_device_t *session, int mode)
/* only does anything in one direction, going to Garmin binary driver */
{
    if (mode == MODE_BINARY) {
	(void)nmea_send(session, "$PGRMC1,1,2,1,,,,2,W,N");
	(void)nmea_send(session, "$PGRMI,,,,,,,R");
	(void)usleep(333);	/* standard Garmin settling time */
	session->gpsdata.dev.driver_mode = MODE_BINARY;
    }
}