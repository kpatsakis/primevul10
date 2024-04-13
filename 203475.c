static void fv18_event_hook(struct gps_device_t *session, event_t event)
{
    if (session->context->readonly)
	return;

    /*
     * Tell an FV18 to send GSAs so we'll know if 3D is accurate.
     * Suppress GLL and VTG.  Enable ZDA so dates will be accurate for replay.
     * It's possible we might not need to redo this on event_reactivate,
     * but doing so is safe and cheap.
     */
    if (event == event_identified || event == event_reactivate)
	(void)nmea_send(session,
			"$PFEC,GPint,GSA01,DTM00,ZDA01,RMC01,GLL00,VTG00,GSV05");
}