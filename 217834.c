check_set_ipv4(struct ofpact_check_params *cp)
{
    ovs_be16 dl_type = get_dl_type(&cp->match->flow);
    if (dl_type != htons(ETH_TYPE_IP)) {
        inconsistent_match(&cp->usable_protocols);
    }
    return 0;
}