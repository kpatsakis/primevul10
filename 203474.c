static gps_mask_t processMTK3301(struct gps_device_t *session)
{
    const char *mtk_reasons[4] =
	{ "Invalid", "Unsupported", "Valid but Failed", "Valid success" };
    gps_mask_t mask;

    /* try a straight NMEA parse, this will set up fields */
    mask = generic_parse_input(session);

    if (session->packet.type == NMEA_PACKET
	&& strncmp(session->driver.nmea.field[0], "PMTK", 4) == 0)
    {
	int msg, reason;

	msg = atoi(&(session->driver.nmea.field[0])[4]);
	switch (msg) {
	case 705:			/*  */
	    (void)strlcat(session->subtype, session->driver.nmea.field[1], sizeof(session->subtype));
	    (void)strlcat(session->subtype, "-", sizeof(session->subtype));
	    (void)strlcat(session->subtype, session->driver.nmea.field[2], sizeof(session->subtype));
	    return ONLINE_SET;
	case 001:			/* ACK / NACK */
	    reason = atoi(session->driver.nmea.field[2]);
	    if (atoi(session->driver.nmea.field[1]) == -1)
		gpsd_report(LOG_WARN, "MTK NACK: unknown sentence\n");
	    else if (reason < 3)
		gpsd_report(LOG_WARN, "MTK NACK: %s, reason: %s\n", session->driver.nmea.field[1],
			    mtk_reasons[reason]);
	    else
		gpsd_report(LOG_WARN, "MTK ACK: %s\n", session->driver.nmea.field[1]);
	    break;
	default:
	    return ONLINE_SET;		/* ignore */
	}
    }

    return mask;
}