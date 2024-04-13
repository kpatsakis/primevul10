parse_set_vlan_pcp(char *arg, bool push_vlan_if_needed,
                   const struct ofpact_parse_params *pp)
{
    struct ofpact_vlan_pcp *vlan_pcp;
    uint8_t pcp;
    char *error;

    error = str_to_u8(arg, "VLAN PCP", &pcp);
    if (error) {
        return error;
    }

    if (pcp & ~7) {
        return xasprintf("%s: not a valid VLAN PCP", arg);
    }
    vlan_pcp = ofpact_put_SET_VLAN_PCP(pp->ofpacts);
    vlan_pcp->vlan_pcp = pcp;
    vlan_pcp->push_vlan_if_needed = push_vlan_if_needed;
    return NULL;
}