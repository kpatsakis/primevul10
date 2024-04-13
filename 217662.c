format_METER(const struct ofpact_meter *a,
             const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%smeter:%s%"PRIu32,
                  colors.param, colors.end, a->meter_id);
}