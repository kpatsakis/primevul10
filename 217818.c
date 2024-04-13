format_SET_IP_ECN(const struct ofpact_ecn *a,
                  const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%smod_nw_ecn:%s%d",
                  colors.param, colors.end, a->ecn);
}