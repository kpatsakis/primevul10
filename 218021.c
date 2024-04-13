encode_DEC_MPLS_TTL(const struct ofpact_null *null OVS_UNUSED,
                    enum ofp_version ofp_version, struct ofpbuf *out)
{
    put_OFPAT_DEC_MPLS_TTL(out, ofp_version);
}