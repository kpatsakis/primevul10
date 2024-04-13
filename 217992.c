format_RESUBMIT(const struct ofpact_resubmit *a,
                const struct ofpact_format_params *fp)
{
    if (a->in_port != OFPP_IN_PORT && a->table_id == 255) {
        ds_put_format(fp->s, "%sresubmit:%s", colors.special, colors.end);
        ofputil_format_port(a->in_port, fp->port_map, fp->s);
    } else {
        ds_put_format(fp->s, "%sresubmit(%s", colors.paren, colors.end);
        if (a->in_port != OFPP_IN_PORT) {
            ofputil_format_port(a->in_port, fp->port_map, fp->s);
        }
        ds_put_char(fp->s, ',');
        if (a->table_id != 255) {
            ofputil_format_table(a->table_id, fp->table_map, fp->s);
        }
        if (a->with_ct_orig) {
            ds_put_cstr(fp->s, ",ct");
        }
        ds_put_format(fp->s, "%s)%s", colors.paren, colors.end);
    }
}