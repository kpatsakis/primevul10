format_SET_TUNNEL(const struct ofpact_tunnel *a,
                  const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sset_tunnel%s:%s%#"PRIx64, colors.param,
                  (a->tun_id > UINT32_MAX
                   || a->ofpact.raw == NXAST_RAW_SET_TUNNEL64 ? "64" : ""),
                  colors.end, a->tun_id);
}