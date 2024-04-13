check_STACK_PUSH(const struct ofpact_stack *a,
                 const struct ofpact_check_params *cp)
{
    return nxm_stack_push_check(a, cp->match);
}