parse_SET_VLAN_PCP(char *arg, const struct ofpact_parse_params *pp)
{
    return parse_set_vlan_pcp(arg, false, pp);
}