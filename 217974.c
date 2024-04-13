check_FIN_TIMEOUT(const struct ofpact_fin_timeout *a OVS_UNUSED,
                  struct ofpact_check_params *cp)
{
    if (cp->match->flow.nw_proto != IPPROTO_TCP) {
        inconsistent_match(&cp->usable_protocols);
    }
    return 0;
}