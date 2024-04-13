format_SET_MPLS_TC(const struct ofpact_mpls_tc *a,
                   const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sset_mpls_ttl(%s%"PRIu8"%s)%s",
                  colors.paren, colors.end, a->tc,
                  colors.paren, colors.end);
}