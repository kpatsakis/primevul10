format_ENQUEUE(const struct ofpact_enqueue *a,
               const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%senqueue:%s", colors.param, colors.end);
    ofputil_format_port(a->port, fp->port_map, fp->s);
    ds_put_format(fp->s, ":%"PRIu32, a->queue);
}