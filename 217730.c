encode_SET_IP_TTL(const struct ofpact_ip_ttl *ttl,
                  enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version >= OFP11_VERSION) {
        put_OFPAT11_SET_NW_TTL(out, ttl->ttl);
    } else {
        struct mf_subfield dst = { .field = mf_from_id(MFF_IP_TTL),
                                   .ofs = 0, .n_bits = 8 };
        put_reg_load(out, &dst, ttl->ttl);
    }
}