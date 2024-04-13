static void path_rewrite(struct gps_device_t *session, char *prefix)
/* prepend the session path to the value of a specified attribute */
{
    /*
     * Hack the packet to reflect its origin.  This code is supposed
     * to insert the path naming the remote gpsd instance into the
     * baginning of the path attribute, followed by a # to separate it
     * from the device.
     */
    char *prefloc;
    for (prefloc = (char *)session->packet.outbuffer;
	 prefloc < (char *)session->packet.outbuffer+session->packet.outbuflen;
	 prefloc++)
	if (strncmp(prefloc, prefix, strlen(prefix)) == 0) {
	    char copy[sizeof(session->packet.outbuffer)];
	    (void)strlcpy(copy,
			  (char *)session->packet.outbuffer,
			  sizeof(copy));
	    prefloc += strlen(prefix);
	    (void)strlcpy(prefloc,
			  session->gpsdata.dev.path,
			  sizeof(session->gpsdata.dev.path));
	    (void)strlcat((char *)session->packet.outbuffer, "#",
			  sizeof(session->packet.outbuffer));
	    (void)strlcat((char *)session->packet.outbuffer,
			  copy + (prefloc-(char *)session->packet.outbuffer),
			  sizeof(session->packet.outbuffer));
	}
    session->packet.outbuflen = strlen((char *)session->packet.outbuffer);
}