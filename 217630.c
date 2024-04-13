format_SET_IPV4_SRC(const struct ofpact_ipv4 *a,
                    const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%smod_nw_src:%s"IP_FMT,
                  colors.param, colors.end, IP_ARGS(a->ipv4));
}