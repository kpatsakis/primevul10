parse_SET_VLAN_VID(char *arg, const struct ofpact_parse_params *pp)
{
    return parse_set_vlan_vid(arg, false, pp);
}