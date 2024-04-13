parse_STACK_PUSH(char *arg, const struct ofpact_parse_params *pp)
{
    return nxm_parse_stack_action(ofpact_put_STACK_PUSH(pp->ofpacts), arg);
}