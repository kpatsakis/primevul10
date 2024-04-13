format_CLEAR_ACTIONS(const struct ofpact_null *a OVS_UNUSED,
                     const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sclear_actions%s", colors.value, colors.end);
}