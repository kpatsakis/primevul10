static gps_mask_t json_pass_packet(struct gps_device_t *session UNUSED)
{
    gpsd_report(LOG_IO, "<= GPS: %s\n", (char *)session->packet.outbuffer);

    /*@-nullpass@*/ /* required only because splint is buggy */
    /* devices and paths need to be edited to */
    if (strstr((char *)session->packet.outbuffer, "DEVICE") != NULL)
	path_rewrite(session, "\"path\":\"");
    path_rewrite(session, "\"device\":\"");

    /* mark certain responses without a path or device attribute */
    if (strstr((char *)session->packet.outbuffer, "VERSION") != NULL
	|| strstr((char *)session->packet.outbuffer, "WATCH") != NULL
	|| strstr((char *)session->packet.outbuffer, "DEVICES") != NULL) {
	session->packet.outbuffer[session->packet.outbuflen-1] = '\0';
	(void)strlcat((char *)session->packet.outbuffer, ",\"remote\":\"",
		      sizeof(session->packet.outbuffer));
	(void)strlcat((char *)session->packet.outbuffer,
		      session->gpsdata.dev.path,
		      sizeof(session->packet.outbuffer));
	(void)strlcat((char *)session->packet.outbuffer, "\"}",
		      sizeof(session->packet.outbuffer));
    }

    gpsd_report (LOG_PROG,
		 "JSON, passing through %s\n",
		 (char *)session->packet.outbuffer);
    /*@-nullpass@*/
    return PASSTHROUGH_IS;
}