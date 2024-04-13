static gps_mask_t rtcm104v3_analyze(struct gps_device_t *session)
{
    uint16_t type = getbeu16(session->packet.inbuffer, 3) >> 4;

    gpsd_report(LOG_RAW, "RTCM 3.x packet %d\n", type);
    rtcm3_unpack(&session->gpsdata.rtcm3, (char *)session->packet.outbuffer);
    session->cycle_end_reliable = true;
    return RTCM3_SET;
}