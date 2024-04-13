format_OUTPUT_TRUNC(const struct ofpact_output_trunc *a,
                    const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%soutput%s(port=", colors.special, colors.end);
    ofputil_format_port(a->port, fp->port_map, fp->s);
    ds_put_format(fp->s, ",max_len=%"PRIu32")", a->max_len);
}