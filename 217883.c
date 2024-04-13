format_NAT(const struct ofpact_nat *a, const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%snat%s", colors.paren, colors.end);

    if (a->flags & (NX_NAT_F_SRC | NX_NAT_F_DST)) {
        ds_put_format(fp->s, "%s(%s", colors.paren, colors.end);
        ds_put_format(fp->s, a->flags & NX_NAT_F_SRC ? "%ssrc%s" : "%sdst%s",
                      colors.param, colors.end);

        if (a->range_af != AF_UNSPEC) {
            ds_put_format(fp->s, "%s=%s", colors.param, colors.end);

            if (a->range_af == AF_INET) {
                ds_put_format(fp->s, IP_FMT, IP_ARGS(a->range.addr.ipv4.min));

                if (a->range.addr.ipv4.max
                    && a->range.addr.ipv4.max != a->range.addr.ipv4.min) {
                    ds_put_format(fp->s, "-"IP_FMT,
                                  IP_ARGS(a->range.addr.ipv4.max));
                }
            } else if (a->range_af == AF_INET6) {
                ipv6_format_addr_bracket(&a->range.addr.ipv6.min, fp->s,
                                        a->range.proto.min);

                if (!ipv6_mask_is_any(&a->range.addr.ipv6.max)
                    && memcmp(&a->range.addr.ipv6.max, &a->range.addr.ipv6.min,
                              sizeof(struct in6_addr)) != 0) {
                    ds_put_char(fp->s, '-');
                    ipv6_format_addr_bracket(&a->range.addr.ipv6.max, fp->s,
                                            a->range.proto.min);
                }
            }
            if (a->range.proto.min) {
                ds_put_char(fp->s, ':');
                ds_put_format(fp->s, "%"PRIu16, a->range.proto.min);

                if (a->range.proto.max
                    && a->range.proto.max != a->range.proto.min) {
                    ds_put_format(fp->s, "-%"PRIu16, a->range.proto.max);
                }
            }
            ds_put_char(fp->s, ',');

            if (a->flags & NX_NAT_F_PERSISTENT) {
                ds_put_format(fp->s, "%spersistent%s,",
                              colors.value, colors.end);
            }
            if (a->flags & NX_NAT_F_PROTO_HASH) {
                ds_put_format(fp->s, "%shash%s,", colors.value, colors.end);
            }
            if (a->flags & NX_NAT_F_PROTO_RANDOM) {
                ds_put_format(fp->s, "%srandom%s,", colors.value, colors.end);
            }
        }
        ds_chomp(fp->s, ',');
        ds_put_format(fp->s, "%s)%s", colors.paren, colors.end);
    }
}