format_SET_L4_SRC_PORT(const struct ofpact_l4_port *a,
                       const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%smod_tp_src:%s%d",
                  colors.param, colors.end, a->port);
}