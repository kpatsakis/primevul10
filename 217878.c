check_CT(struct ofpact_conntrack *a, struct ofpact_check_params *cp)
{
    struct flow *flow = &cp->match->flow;

    if (!dl_type_is_ip_any(get_dl_type(flow))
        || (flow->ct_state & CS_INVALID && a->flags & NX_CT_F_COMMIT)
        || (a->alg == IPPORT_FTP && flow->nw_proto != IPPROTO_TCP)
        || (a->alg == IPPORT_TFTP && flow->nw_proto != IPPROTO_UDP)) {
        /* We can't downgrade to OF1.0 and expect inconsistent CT actions
         * be silently discarded.  Instead, datapath flow install fails, so
         * it is better to flag inconsistent CT actions as hard errors. */
        return OFPERR_OFPBAC_MATCH_INCONSISTENT;
    }

    if (a->zone_src.field) {
        return mf_check_src(&a->zone_src, cp->match);
    }

    return check_subactions(a->actions, ofpact_ct_get_action_len(a), cp);
}