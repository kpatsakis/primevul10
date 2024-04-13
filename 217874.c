encode_SET_VLAN_PCP(const struct ofpact_vlan_pcp *vlan_pcp,
                    enum ofp_version ofp_version, struct ofpbuf *out)
{
    uint8_t pcp = vlan_pcp->vlan_pcp;

    /* Push a VLAN tag, if none is present and this form of the action calls
     * for such a feature. */
    if (ofp_version > OFP10_VERSION
        && vlan_pcp->push_vlan_if_needed
        && !vlan_pcp->flow_has_vlan) {
        put_OFPAT11_PUSH_VLAN(out, htons(ETH_TYPE_VLAN_8021Q));
    }

    if (ofp_version == OFP10_VERSION) {
        put_OFPAT10_SET_VLAN_PCP(out, pcp);
    } else if (ofp_version == OFP11_VERSION) {
        put_OFPAT11_SET_VLAN_PCP(out, pcp);
    } else {
        put_set_field(out, ofp_version, MFF_VLAN_PCP, pcp);
    }
}