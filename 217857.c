format_DEBUG_SLOW(const struct ofpact_null *a OVS_UNUSED,
                  const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sdebug_slow%s", colors.value, colors.end);
}