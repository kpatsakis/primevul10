static void nmea_mode_switch(struct gps_device_t *session, int mode)
{
    /*
     * If the daemon has seen this device in a binary mode, we may
     * actually know how to switch back.
     */
    if (mode == MODE_BINARY)
    {
	const struct gps_type_t **dp;

	/*@-shiftnegative@*/
	for (dp = gpsd_drivers; *dp; dp++) {
	    if ((*dp)->packet_type > 0 && (*dp)->packet_type != session->packet.type &&
	    	    (session->observed & PACKET_TYPEMASK((*dp)->packet_type))!=0) {
		(*dp)->mode_switcher(session, mode);
		break;
	    }
	}
	/*@+shiftnegative@*/
    }
}