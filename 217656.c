decode_OFPAT_RAW11_PUSH_VLAN(ovs_be16 eth_type,
                             enum ofp_version ofp_version OVS_UNUSED,
                             struct ofpbuf *out)
{
    struct ofpact_push_vlan *push_vlan;
    if (!eth_type_vlan(eth_type)) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }
    push_vlan = ofpact_put_PUSH_VLAN(out);
    push_vlan->ethertype = eth_type;
    return 0;
}