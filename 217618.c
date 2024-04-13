check_WRITE_ACTIONS(struct ofpact_nest *a,
                    const struct ofpact_check_params *cp)
{
    /* Use a temporary copy of 'cp' to avoid updating 'cp->usable_protocols',
     * since we can't check consistency of an action set. */
    struct ofpact_check_params tmp = *cp;
    return ofpacts_check(a->actions, ofpact_nest_get_action_len(a), &tmp);
}