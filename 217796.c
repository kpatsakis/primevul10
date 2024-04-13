decode_OFPAT_RAW_SET_MPLS_TTL(uint8_t ttl,
                              enum ofp_version ofp_version OVS_UNUSED,
                              struct ofpbuf *out)
{
    ofpact_put_SET_MPLS_TTL(out)->ttl = ttl;
    return 0;
}