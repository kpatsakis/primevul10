decode_OFPAT_RAW11_SET_NW_TTL(uint8_t ttl,
                              enum ofp_version ofp_version OVS_UNUSED,
                              struct ofpbuf *out)
{
    ofpact_put_SET_IP_TTL(out)->ttl = ttl;
    return 0;
}