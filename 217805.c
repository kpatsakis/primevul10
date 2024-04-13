check_POP_MPLS(const struct ofpact_pop_mpls *a, struct ofpact_check_params *cp)
{
    struct flow *flow = &cp->match->flow;
    ovs_be16 dl_type = get_dl_type(flow);

    if (flow->packet_type != htonl(PT_ETH) || !eth_type_mpls(dl_type)) {
        inconsistent_match(&cp->usable_protocols);
    }
    flow->dl_type = a->ethertype;
    return 0;
}