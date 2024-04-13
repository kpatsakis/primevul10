encode_SET_ETH_addr(const struct ofpact_mac *mac, enum ofp_version ofp_version,
                    enum ofp_raw_action_type raw, enum mf_field_id field,
                    struct ofpbuf *out)
{
    if (ofp_version < OFP12_VERSION) {
        struct ofp_action_dl_addr *oada;

        oada = ofpact_put_raw(out, ofp_version, raw, 0);
        oada->dl_addr = mac->mac;
    } else {
        put_set_field(out, ofp_version, field, eth_addr_to_uint64(mac->mac));
    }
}