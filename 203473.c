static void tripmate_event_hook(struct gps_device_t *session, event_t event)
{
    if (session->context->readonly)
	return;
    /* TripMate requires this response to the ASTRAL it sends at boot time */
    if (event == event_identified)
	(void)nmea_send(session, "$IIGPQ,ASTRAL");
    /* stop it sending PRWIZCH */
    if (event == event_identified || event == event_reactivate)
	(void)nmea_send(session, "$PRWIILOG,ZCH,V,,");
}