check_DEC_NSH_TTL(const struct ofpact_null *a OVS_UNUSED,
                  struct ofpact_check_params *cp)
{
    struct flow *flow = &cp->match->flow;
    if (flow->packet_type != htonl(PT_NSH) &&
        flow->dl_type != htons(ETH_TYPE_NSH)) {
        inconsistent_match(&cp->usable_protocols);
    }
    return 0;
}