static void nmea_event_hook(struct gps_device_t *session, event_t event)
{
    if (session->context->readonly)
	return;
    /*
     * This is where we try to tickle NMEA devices into revealing their
     * inner natures.
     */
    if (event == event_configure) {
	/*
	 * The reason for splitting these probes up by packet sequence
	 * number, interleaving them with the first few packet receives,
	 * is because many generic-NMEA devices get confused if you send
	 * too much at them in one go.
	 *
	 * A fast response to an early probe will change drivers so the
	 * later ones won't be sent at all.  Thus, for best overall
	 * performance, order these to probe for the most popular types
	 * soonest.
	 *
	 * Note: don't make the trigger strings identical to the probe,
	 * because some NMEA devices (notably SiRFs) will just echo
	 * unknown strings right back at you. A useful dodge is to append
	 * a comma to the trigger, because that won't be in the response
	 * unless there is actual following data.
	 */
	switch (session->packet.counter) {
#ifdef NMEA_ENABLE
	case 0:
	    /* probe for Garmin serial GPS -- expect $PGRMC followed by data */
	    gpsd_report(LOG_PROG, "=> Probing for Garmin NMEA\n");
	    (void)nmea_send(session, "$PGRMCE");
	    break;
#endif /* NMEA_ENABLE */
#ifdef SIRF_ENABLE
	case 1:
	    /*
	     * We used to try to probe for SiRF by issuing "$PSRF105,1"
	     * and expecting "$Ack Input105.".  But it turns out this
	     * only works for SiRF-IIs; SiRF-I and SiRF-III don't respond.
	     * Thus the only reliable probe is to try to flip the SiRF into
	     * binary mode, cluing in the library to revert it on close.
	     *
	     * SiRFs dominate the GPS-mouse market, so we used to put this test
	     * first. Unfortunately this causes problems for gpsctl, as it cannot
	     * select the NMEA driver without switching the device back to
	     * binary mode!  Fix this if we ever find a nondisruptive probe string.
	     */
	    gpsd_report(LOG_PROG, "=> Probing for SiRF\n");
	    (void)nmea_send(session,
			    "$PSRF100,0,%d,%d,%d,0",
			    session->gpsdata.dev.baudrate,
			    9 - session->gpsdata.dev.stopbits,
			    session->gpsdata.dev.stopbits);
	    session->back_to_nmea = true;
	    break;
#endif /* SIRF_ENABLE */
#ifdef NMEA_ENABLE
	case 2:
	    /* probe for the FV-18 -- expect $PFEC,GPint followed by data */
	    gpsd_report(LOG_PROG, "=> Probing for FV-18\n");
	    (void)nmea_send(session, "$PFEC,GPint");
	    break;
	case 3:
	    /* probe for the Trimble Copernicus */
	    gpsd_report(LOG_PROG, "=> Probing for Trimble Copernicus\n");
	    (void)nmea_send(session, "$PTNLSNM,0139,01");
	    break;
#endif /* NMEA_ENABLE */
#ifdef EVERMORE_ENABLE
	case 4:
	    gpsd_report(LOG_PROG, "=> Probing for Evermore\n");
	    /* Enable checksum and GGA(1s), GLL(0s), GSA(1s), GSV(1s), RMC(1s), VTG(0s), PEMT101(1s) */
	    /* EverMore will reply with: \x10\x02\x04\x38\x8E\xC6\x10\x03 */
	    (void)gpsd_write(session,
			     "\x10\x02\x12\x8E\x7F\x01\x01\x00\x01\x01\x01\x00\x01\x00\x00\x00\x00\x00\x00\x13\x10\x03",
			     22);
	    break;
#endif /* EVERMORE_ENABLE */
#ifdef GPSCLOCK_ENABLE
	case 5:
	    /* probe for Furuno Electric GH-79L4-N (GPSClock); expect $PFEC,GPssd */
	    gpsd_report(LOG_PROG, "=> Probing for GPSClock\n");
	    (void)nmea_send(session, "$PFEC,GPsrq");
	    break;
#endif /* GPSCLOCK_ENABLE */
#ifdef ASHTECH_ENABLE
	case 6:
	    /* probe for Ashtech -- expect $PASHR,RID */
	    gpsd_report(LOG_PROG, "=> Probing for Ashtech\n");
	    (void)nmea_send(session, "$PASHQ,RID");
	    break;
#endif /* ASHTECH_ENABLE */
#ifdef UBX_ENABLE
	case 7:
	    /* probe for UBX -- query software version */
	    gpsd_report(LOG_PROG, "=> Probing for UBX\n");
	    (void)ubx_write(session, 0x0au, 0x04, NULL, 0);
	    break;
#endif /* UBX_ENABLE */
#ifdef MTK3301_ENABLE
	case 8:
	    /* probe for MTK-3301 -- expect $PMTK705 */
	    gpsd_report(LOG_PROG, "=> Probing for MediaTek\n");
	    (void)nmea_send(session, "$PMTK605");
	    break;
#endif /* MTK3301_ENABLE */
	default:
	    break;
	}
    }
}