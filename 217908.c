encode_SET_VLAN_VID(const struct ofpact_vlan_vid *vlan_vid,
                    enum ofp_version ofp_version, struct ofpbuf *out)
{
    uint16_t vid = vlan_vid->vlan_vid;

    /* Push a VLAN tag, if none is present and this form of the action calls
     * for such a feature. */
    if (ofp_version > OFP10_VERSION
        && vlan_vid->push_vlan_if_needed
        && !vlan_vid->flow_has_vlan) {
        put_OFPAT11_PUSH_VLAN(out, htons(ETH_TYPE_VLAN_8021Q));
    }

    if (ofp_version == OFP10_VERSION) {
        put_OFPAT10_SET_VLAN_VID(out, vid);
    } else if (ofp_version == OFP11_VERSION) {
        put_OFPAT11_SET_VLAN_VID(out, vid);
    } else {
        put_set_field(out, ofp_version, MFF_VLAN_VID, vid | OFPVID12_PRESENT);
    }
}