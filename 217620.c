format_SET_QUEUE(const struct ofpact_queue *a,
                 const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sset_queue:%s%"PRIu32,
                  colors.param, colors.end, a->queue_id);
}