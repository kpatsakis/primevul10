encode_SET_IP_DSCP(const struct ofpact_dscp *dscp,
                   enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version < OFP12_VERSION) {
        put_OFPAT_SET_NW_TOS(out, ofp_version, dscp->dscp);
    } else {
        put_set_field(out, ofp_version, MFF_IP_DSCP_SHIFTED, dscp->dscp >> 2);
    }
}