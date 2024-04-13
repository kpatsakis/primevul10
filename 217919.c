format_CT_CLEAR(const struct ofpact_null *a OVS_UNUSED,
                const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sct_clear%s", colors.value, colors.end);
}