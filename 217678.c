format_POP_QUEUE(const struct ofpact_null *a OVS_UNUSED,
                 const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%spop_queue%s", colors.value, colors.end);
}