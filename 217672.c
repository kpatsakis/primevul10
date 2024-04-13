decode_OFPAT_RAW_SET_MPLS_LABEL(ovs_be32 label,
                                enum ofp_version ofp_version OVS_UNUSED,
                                struct ofpbuf *out)
{
    ofpact_put_SET_MPLS_LABEL(out)->label = label;
    return 0;
}