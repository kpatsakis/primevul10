check_NAT(const struct ofpact_nat *a, const struct ofpact_check_params *cp)
{
    ovs_be16 dl_type = get_dl_type(&cp->match->flow);
    if (!dl_type_is_ip_any(dl_type) ||
        (a->range_af == AF_INET && dl_type != htons(ETH_TYPE_IP)) ||
        (a->range_af == AF_INET6 && dl_type != htons(ETH_TYPE_IPV6))) {
        return OFPERR_OFPBAC_MATCH_INCONSISTENT;
    }
    return 0;
}