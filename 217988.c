format_DEC_MPLS_TTL(const struct ofpact_null *a OVS_UNUSED,
                    const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sdec_mpls_ttl%s", colors.value, colors.end);
}