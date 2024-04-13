encode_SET_IP_ECN(const struct ofpact_ecn *ip_ecn,
                  enum ofp_version ofp_version, struct ofpbuf *out)
{
    uint8_t ecn = ip_ecn->ecn;
    if (ofp_version == OFP10_VERSION) {
        struct mf_subfield dst = { .field = mf_from_id(MFF_IP_ECN),
                                   .ofs = 0, .n_bits = 2 };
        put_reg_load(out, &dst, ecn);
    } else if (ofp_version == OFP11_VERSION) {
        put_OFPAT11_SET_NW_ECN(out, ecn);
    } else {
        put_set_field(out, ofp_version, MFF_IP_ECN, ecn);
    }
}