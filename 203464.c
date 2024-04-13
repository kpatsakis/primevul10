static void ashtech_event_hook(struct gps_device_t *session, event_t event)
{
    if (session->context->readonly)
	return;

    if (event == event_wakeup)
	(void)nmea_send(session, "$PASHQ,RID");
    if (event == event_identified) {
	/* turn WAAS on. can't hurt... */
	(void)nmea_send(session, "$PASHS,WAS,ON");
	/* reset to known output state */
	(void)nmea_send(session, "$PASHS,NME,ALL,A,OFF");
	/* then turn on some useful sentences */
#ifdef __future__
	/* we could parse these, but they're oversize so they get dropped */
	(void)nmea_send(session, "$PASHS,NME,POS,A,ON");
	(void)nmea_send(session, "$PASHS,NME,SAT,A,ON");
#else
	(void)nmea_send(session, "$PASHS,NME,GGA,A,ON");
	(void)nmea_send(session, "$PASHS,NME,GSA,A,ON");
	(void)nmea_send(session, "$PASHS,NME,GSV,A,ON");
	(void)nmea_send(session, "$PASHS,NME,RMC,A,ON");
#endif
	(void)nmea_send(session, "$PASHS,NME,ZDA,A,ON");
    }
}