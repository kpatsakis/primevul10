format_NOTE(const struct ofpact_note *a,
            const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%snote:%s", colors.param, colors.end);
    format_hex_arg(fp->s, a->data, a->length);
}