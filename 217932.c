format_LEARN(const struct ofpact_learn *a,
             const struct ofpact_format_params *fp)
{
    learn_format(a, fp->port_map, fp->table_map, fp->s);
}