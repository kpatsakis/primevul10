check_set_mpls(struct ofpact_check_params *cp)
{
    ovs_be16 dl_type = get_dl_type(&cp->match->flow);
    if (!eth_type_mpls(dl_type)) {
        inconsistent_match(&cp->usable_protocols);
    }
    return 0;
}