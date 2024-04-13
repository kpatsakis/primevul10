format_GROUP(const struct ofpact_group *a,
             const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sgroup:%s%"PRIu32,
                  colors.special, colors.end, a->group_id);
}