format_STACK_POP(const struct ofpact_stack *a,
                 const struct ofpact_format_params *fp)
{
    nxm_format_stack_pop(a, fp->s);
}