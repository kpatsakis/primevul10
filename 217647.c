format_CLONE(const struct ofpact_nest *a,
             const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sclone(%s", colors.paren, colors.end);
    ofpacts_format(a->actions, ofpact_nest_get_action_len(a), fp);
    ds_put_format(fp->s, "%s)%s", colors.paren, colors.end);
}