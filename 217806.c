check_RESUBMIT(const struct ofpact_resubmit *a,
               const struct ofpact_check_params *cp)
{
    if (a->with_ct_orig && !is_ct_valid(&cp->match->flow, &cp->match->wc,
                                        NULL)) {
        return OFPERR_OFPBAC_MATCH_INCONSISTENT;
    }
    return 0;
}