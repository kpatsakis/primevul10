format_SET_MPLS_LABEL(const struct ofpact_mpls_label *a,
                      const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sset_mpls_label(%s%"PRIu32"%s)%s",
                  colors.paren, colors.end, ntohl(a->label),
                  colors.paren, colors.end);
}