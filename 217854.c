decode_OFPAT_RAW_PUSH_MPLS(ovs_be16 ethertype,
                           enum ofp_version ofp_version OVS_UNUSED,
                           struct ofpbuf *out)
{
    struct ofpact_push_mpls *oam;

    if (!eth_type_mpls(ethertype)) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }
    oam = ofpact_put_PUSH_MPLS(out);
    oam->ethertype = ethertype;

    return 0;
}