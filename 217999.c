check_STRIP_VLAN(const struct ofpact_null *a OVS_UNUSED,
                 struct ofpact_check_params *cp)
{
    if (!(cp->match->flow.vlans[0].tci & htons(VLAN_CFI))) {
        inconsistent_match(&cp->usable_protocols);
    }
    flow_pop_vlan(&cp->match->flow, NULL);
    return 0;
}