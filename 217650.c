encode_SET_IPV4_DST(const struct ofpact_ipv4 *ipv4,
                    enum ofp_version ofp_version, struct ofpbuf *out)
{
    encode_SET_IPV4_addr(ipv4, ofp_version, OFPAT_RAW_SET_NW_DST, MFF_IPV4_DST,
                         out);
}