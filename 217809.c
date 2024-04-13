check_STACK_POP(const struct ofpact_stack *a,
                const struct ofpact_check_params *cp)
{
    return nxm_stack_pop_check(a, cp->match);
}