parse_PUSH_VLAN(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_push_vlan *push_vlan;
    uint16_t ethertype;
    char *error;

    *pp->usable_protocols &= OFPUTIL_P_OF11_UP;
    error = str_to_u16(arg, "ethertype", &ethertype);
    if (error) {
        return error;
    }

    if (!eth_type_vlan(htons(ethertype))) {
        return xasprintf("%s: not a valid VLAN ethertype", arg);
    }
    push_vlan = ofpact_put_PUSH_VLAN(pp->ofpacts);
    push_vlan->ethertype = htons(ethertype);
    return NULL;
}