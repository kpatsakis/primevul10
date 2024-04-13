static void earthmate_event_hook(struct gps_device_t *session, event_t event)
{
    if (session->context->readonly)
	return;
    if (event == event_triggermatch) {
	(void)gpsd_write(session, "EARTHA\r\n", 8);
	(void)usleep(10000);
	(void)gpsd_switch_driver(session, "Zodiac Binary");
    }
}