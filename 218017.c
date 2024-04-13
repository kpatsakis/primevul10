ofpacts_check(struct ofpact ofpacts[], size_t ofpacts_len,
              struct ofpact_check_params *cp)
{
    /* Save fields that might temporarily be modified. */
    struct flow *flow = &cp->match->flow;
    ovs_be32 packet_type = flow->packet_type;
    ovs_be16 dl_type = flow->dl_type;
    uint8_t nw_proto = flow->nw_proto;
    union flow_vlan_hdr vlans[FLOW_MAX_VLAN_HEADERS];
    memcpy(vlans, flow->vlans, sizeof vlans);

    /* Check all the actions. */
    cp->usable_protocols = OFPUTIL_P_ANY;
    enum ofperr error = 0;
    struct ofpact *a;
    OFPACT_FOR_EACH (a, ofpacts, ofpacts_len) {
        error = ofpact_check__(a, cp);
        if (error) {
            break;
        }
    }

    /* Restore fields that may have been modified. */
    flow->packet_type = packet_type;
    flow->dl_type = dl_type;
    memcpy(flow->vlans, vlans, sizeof vlans);
    flow->nw_proto = nw_proto;

    return error;
}