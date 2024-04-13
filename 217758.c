check_PUSH_MPLS(const struct ofpact_push_mpls *a,
                struct ofpact_check_params *cp)
{
    struct flow *flow = &cp->match->flow;

    if (flow->packet_type != htonl(PT_ETH)) {
        inconsistent_match(&cp->usable_protocols);
    }
    flow->dl_type = a->ethertype;

    /* The packet is now MPLS and the MPLS payload is opaque.
     * Thus nothing can be assumed about the network protocol.
     * Temporarily mark that we have no nw_proto. */
    flow->nw_proto = 0;

    return 0;
}