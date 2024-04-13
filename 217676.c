check_SET_VLAN_PCP(struct ofpact_vlan_pcp *a, struct ofpact_check_params *cp)
{
    /* Remember if we saw a vlan tag in the flow to aid translating to OpenFlow
     * 1.1+ if need be. */
    ovs_be16 *tci = &cp->match->flow.vlans[0].tci;
    a->flow_has_vlan = (*tci & htons(VLAN_CFI)) != 0;
    if (!a->flow_has_vlan && !a->push_vlan_if_needed) {
        inconsistent_match(&cp->usable_protocols);
    }

    /* Temporarily mark that we have a vlan tag. */
    *tci |= htons(VLAN_CFI);

    return 0;
}