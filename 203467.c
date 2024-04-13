static gps_mask_t rtcm104v2_analyze(struct gps_device_t *session)
{
    rtcm2_unpack(&session->gpsdata.rtcm2, (char *)session->packet.isgps.buf);
    /* extra guard prevents expensive hexdump calls */
    if (session->context->debug >= LOG_RAW)
	gpsd_report(LOG_RAW, "RTCM 2.x packet type 0x%02x length %d words from %zd bytes: %s\n",
		    session->gpsdata.rtcm2.type,
		    session->gpsdata.rtcm2.length + 2,
		    session->packet.isgps.buflen,
		    gpsd_hexdump((char *)session->packet.isgps.buf,
				 (session->gpsdata.rtcm2.length +
				  2) * sizeof(isgps30bits_t)));
    session->cycle_end_reliable = true;
    return RTCM2_SET;
}