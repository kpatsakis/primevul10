format_STACK_PUSH(const struct ofpact_stack *a,
                  const struct ofpact_format_params *fp)
{
    nxm_format_stack_push(a, fp->s);
}