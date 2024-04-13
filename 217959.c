check_PUSH_VLAN(const struct ofpact_push_vlan *a OVS_UNUSED,
                struct ofpact_check_params *cp)
{
    struct flow *flow = &cp->match->flow;
    if (flow->vlans[FLOW_MAX_VLAN_HEADERS - 1].tci & htons(VLAN_CFI)) {
        /* Support maximum (FLOW_MAX_VLAN_HEADERS) VLAN headers. */
        return OFPERR_OFPBAC_BAD_TAG;
    }
    /* Temporary mark that we have a vlan tag. */
    flow_push_vlan_uninit(flow, NULL);
    flow->vlans[0].tci |= htons(VLAN_CFI);
    return 0;
}