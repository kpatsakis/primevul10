format_SET_IP_TTL(const struct ofpact_ip_ttl *a,
                  const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%smod_nw_ttl:%s%d",
                  colors.param, colors.end, a->ttl);
}