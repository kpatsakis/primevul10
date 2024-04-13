format_SET_ETH_SRC(const struct ofpact_mac *a,
                   const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%smod_dl_src:%s"ETH_ADDR_FMT,
                  colors.param, colors.end, ETH_ADDR_ARGS(a->mac));
}