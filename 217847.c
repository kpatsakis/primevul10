encode_SET_MPLS_TC(const struct ofpact_mpls_tc *tc,
                   enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version < OFP12_VERSION) {
        put_OFPAT_SET_MPLS_TC(out, ofp_version, tc->tc);
    } else {
        put_set_field(out, ofp_version, MFF_MPLS_TC, tc->tc);
    }
}