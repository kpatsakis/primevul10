format_CONJUNCTION(const struct ofpact_conjunction *oc,
                   const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sconjunction(%s%"PRIu32",%d/%"PRIu8"%s)%s",
                  colors.paren, colors.end,
                  oc->id, oc->clause + 1, oc->n_clauses,
                  colors.paren, colors.end);
}