format_GOTO_TABLE(const struct ofpact_goto_table *a,
                  const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sgoto_table:%s", colors.param, colors.end);
    ofputil_format_table(a->table_id, fp->table_map, fp->s);
}