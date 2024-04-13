static void gpsclock_event_hook(struct gps_device_t *session, event_t event)
{
    if (session->context->readonly)
	return;
    /*
     * Michael St. Laurent <mikes@hartwellcorp.com> reports that you have to
     * ignore the trailing PPS edge when extracting time from this chip.
     */
    if (event == event_identified || event == event_reactivate) {
	gpsd_report(LOG_INF, "PPS trailing edge will be ignored\n");
	session->driver.nmea.ignore_trailing_edge = true;
    }
}