static gps_mask_t aivdm_analyze(struct gps_device_t *session)
{
    if (session->packet.type == AIVDM_PACKET) {
	if (aivdm_decode
	    ((char *)session->packet.outbuffer, session->packet.outbuflen,
	     session->aivdm, &session->gpsdata.ais, session->context->debug)) {
	    return ONLINE_SET | AIS_SET;
	} else
	    return ONLINE_SET;
#ifdef NMEA_ENABLE
    } else if (session->packet.type == NMEA_PACKET) {
	return nmea_parse((char *)session->packet.outbuffer, session);
#endif /* NMEA_ENABLE */
    } else
	return 0;
}