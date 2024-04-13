encode_SET_MPLS_TTL(const struct ofpact_mpls_ttl *ttl,
                    enum ofp_version ofp_version, struct ofpbuf *out)
{
    put_OFPAT_SET_MPLS_TTL(out, ofp_version, ttl->ttl);
}