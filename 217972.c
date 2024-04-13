decode_set_vlan_vid(uint16_t vid, bool push_vlan_if_needed, struct ofpbuf *out)
{
    if (vid & ~0xfff) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    } else {
        struct ofpact_vlan_vid *vlan_vid = ofpact_put_SET_VLAN_VID(out);
        vlan_vid->vlan_vid = vid;
        vlan_vid->push_vlan_if_needed = push_vlan_if_needed;
        return 0;
    }
}