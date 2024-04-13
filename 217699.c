decode_OFPAT_RAW_SET_MPLS_TC(uint8_t tc,
                             enum ofp_version ofp_version OVS_UNUSED,
                             struct ofpbuf *out)
{
    ofpact_put_SET_MPLS_TC(out)->tc = tc;
    return 0;
}