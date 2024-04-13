check_SET_FIELD(struct ofpact_set_field *a,
                const struct ofpact_check_params *cp)
{
    const struct mf_field *mf = a->field;
    struct flow *flow = &cp->match->flow;
    ovs_be16 *tci = &flow->vlans[0].tci;

    /* Require OXM_OF_VLAN_VID to have an existing VLAN header. */
    if (!mf_are_prereqs_ok(mf, flow, NULL)
        || (mf->id == MFF_VLAN_VID && !(*tci & htons(VLAN_CFI)))) {
        VLOG_WARN_RL(&rl, "set_field %s lacks correct prerequisites",
                     mf->name);
        return OFPERR_OFPBAC_MATCH_INCONSISTENT;
    }

    /* Remember if we saw a VLAN tag in the flow, to aid translating to
     * OpenFlow 1.1 if need be. */
    a->flow_has_vlan = (*tci & htons(VLAN_CFI)) != 0;
    if (mf->id == MFF_VLAN_TCI) {
        /* The set field may add or remove the VLAN tag,
         * Mark the status temporarily. */
        *tci = a->value->be16;
    }

    return 0;
}