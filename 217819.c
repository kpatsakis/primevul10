decode_OFPAT_RAW_SET_TP_SRC(ovs_be16 port,
                            enum ofp_version ofp_version OVS_UNUSED,
                            struct ofpbuf *out)
{
    ofpact_put_SET_L4_SRC_PORT(out)->port = ntohs(port);
    return 0;
}