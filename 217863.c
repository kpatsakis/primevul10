parse_set_vlan_vid(char *arg, bool push_vlan_if_needed,
                   const struct ofpact_parse_params *pp)
{
    struct ofpact_vlan_vid *vlan_vid;
    uint16_t vid;
    char *error;

    error = str_to_u16(arg, "VLAN VID", &vid);
    if (error) {
        return error;
    }

    if (vid & ~VLAN_VID_MASK) {
        return xasprintf("%s: not a valid VLAN VID", arg);
    }
    vlan_vid = ofpact_put_SET_VLAN_VID(pp->ofpacts);
    vlan_vid->vlan_vid = vid;
    vlan_vid->push_vlan_if_needed = push_vlan_if_needed;
    return NULL;
}