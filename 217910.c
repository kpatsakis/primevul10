encode_NAT(const struct ofpact_nat *nat,
           enum ofp_version ofp_version OVS_UNUSED,
           struct ofpbuf *out)
{
    struct nx_action_nat *nan;
    const size_t ofs = out->size;
    uint16_t range_present = 0;

    nan = put_NXAST_NAT(out);
    nan->flags = htons(nat->flags);
    if (nat->range_af == AF_INET) {
        if (nat->range.addr.ipv4.min) {
            ovs_be32 *min = ofpbuf_put_uninit(out, sizeof *min);
            *min = nat->range.addr.ipv4.min;
            range_present |= NX_NAT_RANGE_IPV4_MIN;
        }
        if (nat->range.addr.ipv4.max) {
            ovs_be32 *max = ofpbuf_put_uninit(out, sizeof *max);
            *max = nat->range.addr.ipv4.max;
            range_present |= NX_NAT_RANGE_IPV4_MAX;
        }
    } else if (nat->range_af == AF_INET6) {
        if (!ipv6_mask_is_any(&nat->range.addr.ipv6.min)) {
            struct in6_addr *min = ofpbuf_put_uninit(out, sizeof *min);
            *min = nat->range.addr.ipv6.min;
            range_present |= NX_NAT_RANGE_IPV6_MIN;
        }
        if (!ipv6_mask_is_any(&nat->range.addr.ipv6.max)) {
            struct in6_addr *max = ofpbuf_put_uninit(out, sizeof *max);
            *max = nat->range.addr.ipv6.max;
            range_present |= NX_NAT_RANGE_IPV6_MAX;
        }
    }
    if (nat->range_af != AF_UNSPEC) {
        if (nat->range.proto.min) {
            ovs_be16 *min = ofpbuf_put_uninit(out, sizeof *min);
            *min = htons(nat->range.proto.min);
            range_present |= NX_NAT_RANGE_PROTO_MIN;
        }
        if (nat->range.proto.max) {
            ovs_be16 *max = ofpbuf_put_uninit(out, sizeof *max);
            *max = htons(nat->range.proto.max);
            range_present |= NX_NAT_RANGE_PROTO_MAX;
        }
    }
    pad_ofpat(out, ofs);
    nan = ofpbuf_at(out, ofs, sizeof *nan);
    nan->range_present = htons(range_present);
}