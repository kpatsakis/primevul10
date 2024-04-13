encode_PUSH_MPLS(const struct ofpact_push_mpls *push_mpls,
                 enum ofp_version ofp_version, struct ofpbuf *out)
{
    put_OFPAT_PUSH_MPLS(out, ofp_version, push_mpls->ethertype);
}