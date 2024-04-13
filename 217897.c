check_DECAP(const struct ofpact_decap *a OVS_UNUSED,
            struct ofpact_check_params *cp)
{
    struct flow *flow = &cp->match->flow;
    if (flow->packet_type == htonl(PT_ETH)) {
        /* Adjust the packet_type to allow subsequent actions. */
        flow->packet_type = PACKET_TYPE_BE(OFPHTN_ETHERTYPE,
                                           ntohs(flow->dl_type));
    } else {
        /* The actual packet_type is only known after decapsulation.
         * Do not allow subsequent actions that depend on packet headers. */
        flow->packet_type = htonl(PT_UNKNOWN);
        flow->dl_type = OVS_BE16_MAX;
    }
    return 0;
}