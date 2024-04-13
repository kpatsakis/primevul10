encode_SET_IPV4_addr(const struct ofpact_ipv4 *ipv4,
                     enum ofp_version ofp_version,
                     enum ofp_raw_action_type raw, enum mf_field_id field,
                     struct ofpbuf *out)
{
    ovs_be32 addr = ipv4->ipv4;
    if (ofp_version < OFP12_VERSION) {
        ofpact_put_raw(out, ofp_version, raw, ntohl(addr));
    } else {
        put_set_field(out, ofp_version, field, ntohl(addr));
    }
}