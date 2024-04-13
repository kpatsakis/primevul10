static void aivdm_event_hook(struct gps_device_t *session, event_t event)
{
    if (event == event_configure)
	/*@i1@*/session->aivdm->type24_queue.index = 0;
}