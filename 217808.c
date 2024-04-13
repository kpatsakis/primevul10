format_UNROLL_XLATE(const struct ofpact_unroll_xlate *a,
                    const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sunroll_xlate(%s%stable=%s",
                  colors.paren,   colors.end,
                  colors.special, colors.end);
    ofputil_format_table(a->rule_table_id, fp->table_map, fp->s);
    ds_put_format(fp->s, ", %scookie=%s%"PRIu64"%s)%s",
                  colors.param,   colors.end, ntohll(a->rule_cookie),
                  colors.paren,   colors.end);
}