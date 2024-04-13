format_EXIT(const struct ofpact_null *a OVS_UNUSED,
            const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sexit%s", colors.special, colors.end);
}