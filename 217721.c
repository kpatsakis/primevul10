format_ENCAP(const struct ofpact_encap *a,
             const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sencap(%s", colors.paren, colors.end);
    ds_put_format(fp->s, "%s", format_encap_pkt_type(a->new_pkt_type));
    if (a->n_props > 0) {
        ds_put_format(fp->s, "%s(%s", colors.paren, colors.end);
        format_ed_props(fp->s, a->n_props, a->props);
        ds_put_format(fp->s, "%s)%s", colors.paren, colors.end);
    }
    ds_put_format(fp->s, "%s)%s", colors.paren, colors.end);
}