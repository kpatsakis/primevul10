encode_PUSH_VLAN(const struct ofpact_push_vlan *push_vlan,
                 enum ofp_version ofp_version, struct ofpbuf *out)
{
    if (ofp_version == OFP10_VERSION) {
        /* PUSH is a side effect of a SET_VLAN_VID/PCP, which should
         * follow this action. */
    } else {
        put_OFPAT11_PUSH_VLAN(out, push_vlan->ethertype);
    }
}