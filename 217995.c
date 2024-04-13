decode_OFPAT_RAW_SET_NW_SRC(ovs_be32 ipv4,
                            enum ofp_version ofp_version OVS_UNUSED,
                            struct ofpbuf *out)
{
    ofpact_put_SET_IPV4_SRC(out)->ipv4 = ipv4;
    return 0;
}