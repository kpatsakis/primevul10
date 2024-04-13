encode_SET_MPLS_LABEL(const struct ofpact_mpls_label *label,
                      enum ofp_version ofp_version,
                                  struct ofpbuf *out)
{
    if (ofp_version < OFP12_VERSION) {
        put_OFPAT_SET_MPLS_LABEL(out, ofp_version, label->label);
    } else {
        put_set_field(out, ofp_version, MFF_MPLS_LABEL, ntohl(label->label));
    }
}