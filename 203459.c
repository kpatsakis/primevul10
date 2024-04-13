static void fury_event_hook(struct gps_device_t *session, event_t event)
{
    if (event == event_wakeup && gpsd_get_speed(session) == 115200)
	(void)fury_rate_switcher(session, 1.0);
    else if (event == event_deactivate)
	(void)fury_rate_switcher(session, 0.0);
}