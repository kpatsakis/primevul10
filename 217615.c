decode_OFPAT_RAW_SET_TP_DST(ovs_be16 port,
                            enum ofp_version ofp_version OVS_UNUSED,
                            struct ofpbuf *out)
{
    ofpact_put_SET_L4_DST_PORT(out)->port = ntohs(port);
    return 0;
}