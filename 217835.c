check_ENCAP(const struct ofpact_encap *a, struct ofpact_check_params *cp)
{
    struct flow *flow = &cp->match->flow;
    flow->packet_type = a->new_pkt_type;
    if (pt_ns(flow->packet_type) == OFPHTN_ETHERTYPE) {
        flow->dl_type = htons(pt_ns_type(flow->packet_type));
    }
    if (!is_ip_any(flow)) {
        flow->nw_proto = 0;
    }
    return 0;
}