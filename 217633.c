format_SET_IP_DSCP(const struct ofpact_dscp *a,
                   const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%smod_nw_tos:%s%d",
                  colors.param, colors.end, a->dscp);
}