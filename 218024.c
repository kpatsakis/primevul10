parse_STACK_POP(char *arg, const struct ofpact_parse_params *pp)
{
    return nxm_parse_stack_action(ofpact_put_STACK_POP(pp->ofpacts), arg);
}