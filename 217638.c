decode_OFPAT_RAW_POP_MPLS(ovs_be16 ethertype,
                          enum ofp_version ofp_version OVS_UNUSED,
                          struct ofpbuf *out)
{
    ofpact_put_POP_MPLS(out)->ethertype = ethertype;
    return 0;
}