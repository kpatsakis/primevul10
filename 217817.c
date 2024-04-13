decode_OFPAT_RAW11_SET_VLAN_VID(uint16_t vid,
                                enum ofp_version ofp_version OVS_UNUSED,
                                struct ofpbuf *out)
{
    return decode_set_vlan_vid(vid, false, out);
}