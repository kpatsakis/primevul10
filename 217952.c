format_DEC_NSH_TTL(const struct ofpact_null *a OVS_UNUSED,
                   const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sdec_nsh_ttl%s", colors.special, colors.end);
}