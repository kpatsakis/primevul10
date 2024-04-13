format_DECAP(const struct ofpact_decap *a,
             const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%sdecap(%s", colors.paren, colors.end);
    if (a->new_pkt_type != htonl(PT_USE_NEXT_PROTO)) {
        ds_put_format(fp->s, "packet_type(ns=%"PRIu16",id=%#"PRIx16")",
                      pt_ns(a->new_pkt_type),
                      pt_ns_type(a->new_pkt_type));
    }
    ds_put_format(fp->s, "%s)%s", colors.paren, colors.end);
}