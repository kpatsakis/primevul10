decode_NXAST_RAW_NAT(const struct nx_action_nat *nan,
                     enum ofp_version ofp_version OVS_UNUSED,
                     struct ofpbuf *out)
{
    struct ofpact_nat *nat;
    uint16_t range_present = ntohs(nan->range_present);
    const char *opts = (char *)(nan + 1);
    uint16_t len = ntohs(nan->len) - sizeof *nan;

    nat = ofpact_put_NAT(out);
    nat->flags = ntohs(nan->flags);

    /* Check for unknown or mutually exclusive flags. */
    if ((nat->flags & ~NX_NAT_F_MASK)
        || (nat->flags & NX_NAT_F_SRC && nat->flags & NX_NAT_F_DST)
        || (nat->flags & NX_NAT_F_PROTO_HASH
            && nat->flags & NX_NAT_F_PROTO_RANDOM)) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

#define NX_NAT_GET_OPT(DST, SRC, LEN, TYPE)                     \
    (LEN >= sizeof(TYPE)                                        \
     ? (memcpy(DST, SRC, sizeof(TYPE)), LEN -= sizeof(TYPE),    \
        SRC += sizeof(TYPE))                                    \
     : NULL)

    nat->range_af = AF_UNSPEC;
    if (range_present & NX_NAT_RANGE_IPV4_MIN) {
        if (range_present & (NX_NAT_RANGE_IPV6_MIN | NX_NAT_RANGE_IPV6_MAX)) {
            return OFPERR_OFPBAC_BAD_ARGUMENT;
        }

        if (!NX_NAT_GET_OPT(&nat->range.addr.ipv4.min, opts, len, ovs_be32)
            || !nat->range.addr.ipv4.min) {
            return OFPERR_OFPBAC_BAD_ARGUMENT;
        }

        nat->range_af = AF_INET;

        if (range_present & NX_NAT_RANGE_IPV4_MAX) {
            if (!NX_NAT_GET_OPT(&nat->range.addr.ipv4.max, opts, len,
                                ovs_be32)) {
                return OFPERR_OFPBAC_BAD_ARGUMENT;
            }
            if (ntohl(nat->range.addr.ipv4.max)
                < ntohl(nat->range.addr.ipv4.min)) {
                return OFPERR_OFPBAC_BAD_ARGUMENT;
            }
        }
    } else if (range_present & NX_NAT_RANGE_IPV4_MAX) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    } else if (range_present & NX_NAT_RANGE_IPV6_MIN) {
        if (!NX_NAT_GET_OPT(&nat->range.addr.ipv6.min, opts, len,
                            struct in6_addr)
            || ipv6_mask_is_any(&nat->range.addr.ipv6.min)) {
            return OFPERR_OFPBAC_BAD_ARGUMENT;
        }

        nat->range_af = AF_INET6;

        if (range_present & NX_NAT_RANGE_IPV6_MAX) {
            if (!NX_NAT_GET_OPT(&nat->range.addr.ipv6.max, opts, len,
                                struct in6_addr)) {
                return OFPERR_OFPBAC_BAD_ARGUMENT;
            }
            if (memcmp(&nat->range.addr.ipv6.max, &nat->range.addr.ipv6.min,
                       sizeof(struct in6_addr)) < 0) {
                return OFPERR_OFPBAC_BAD_ARGUMENT;
            }
        }
    } else if (range_present & NX_NAT_RANGE_IPV6_MAX) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    if (range_present & NX_NAT_RANGE_PROTO_MIN) {
        ovs_be16 proto;

        if (nat->range_af == AF_UNSPEC) {
            return OFPERR_OFPBAC_BAD_ARGUMENT;
        }
        if (!NX_NAT_GET_OPT(&proto, opts, len, ovs_be16) || proto == 0) {
            return OFPERR_OFPBAC_BAD_ARGUMENT;
        }
        nat->range.proto.min = ntohs(proto);
        if (range_present & NX_NAT_RANGE_PROTO_MAX) {
            if (!NX_NAT_GET_OPT(&proto, opts, len, ovs_be16)) {
                return OFPERR_OFPBAC_BAD_ARGUMENT;
            }
            nat->range.proto.max = ntohs(proto);
            if (nat->range.proto.max < nat->range.proto.min) {
                return OFPERR_OFPBAC_BAD_ARGUMENT;
            }
        }
    } else if (range_present & NX_NAT_RANGE_PROTO_MAX) {
        return OFPERR_OFPBAC_BAD_ARGUMENT;
    }

    return 0;
}