gps_mask_t generic_parse_input(struct gps_device_t *session)
{
    const struct gps_type_t **dp;

    if (session->packet.type == COMMENT_PACKET) {
	gpsd_set_century(session);
	return 0;
#ifdef NMEA_ENABLE
    } else if (session->packet.type == NMEA_PACKET) {
	gps_mask_t st = 0;
	char *sentence = (char *)session->packet.outbuffer;

	if (sentence[strlen(sentence)-1] != '\n')
	    gpsd_report(LOG_IO, "<= GPS: %s\n", sentence);
	else
	    gpsd_report(LOG_IO, "<= GPS: %s", sentence);

	if ((st=nmea_parse(sentence, session)) == 0) {
	    gpsd_report(LOG_WARN, "unknown sentence: \"%s\"\n",	sentence);
	}
	for (dp = gpsd_drivers; *dp; dp++) {
	    char *trigger = (*dp)->trigger;

	    if (trigger!=NULL && strncmp(sentence,trigger,strlen(trigger))==0) {
		gpsd_report(LOG_PROG, "found trigger string %s.\n", trigger);
		if (*dp != session->device_type) {
		    (void)gpsd_switch_driver(session, (*dp)->type_name);
		    if (session->device_type != NULL
			&& session->device_type->event_hook != NULL)
			session->device_type->event_hook(session,
							 event_triggermatch);
		    st |= DEVICEID_SET;
		}
	    }
	}
	return st;
#endif /* NMEA_ENABLE */
    } else {
	for (dp = gpsd_drivers; *dp; dp++) {
	    if (session->packet.type == (*dp)->packet_type) {
		(void)gpsd_switch_driver(session, (*dp)->type_name);
		return (*dp)->parse_packet(session);
	    }
	}
	return 0;
    }
}