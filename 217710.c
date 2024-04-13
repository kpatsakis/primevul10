encode_SET_L4_port(const struct ofpact_l4_port *l4_port,
                   enum ofp_version ofp_version, enum ofp_raw_action_type raw,
                   enum mf_field_id field, struct ofpbuf *out)
{
    uint16_t port = l4_port->port;

    if (ofp_version >= OFP12_VERSION && field != MFF_N_IDS) {
        put_set_field(out, ofp_version, field, port);
    } else {
        ofpact_put_raw(out, ofp_version, raw, port);
    }
}