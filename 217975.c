format_DEBUG_RECIRC(const struct ofpact_null *a OVS_UNUSED,
                    const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sdebug_recirc%s", colors.value, colors.end);
}