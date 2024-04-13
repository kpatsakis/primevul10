encode_SET_ETH_DST(const struct ofpact_mac *mac,
                               enum ofp_version ofp_version,
                               struct ofpbuf *out)
{
    encode_SET_ETH_addr(mac, ofp_version, OFPAT_RAW_SET_DL_DST, MFF_ETH_DST,
                        out);

}