decode_OFPAT_RAW_SET_QUEUE(uint32_t queue_id,
                           enum ofp_version ofp_version OVS_UNUSED,
                           struct ofpbuf *out)
{
    ofpact_put_SET_QUEUE(out)->queue_id = queue_id;
    return 0;
}