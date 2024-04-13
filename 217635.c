decode_NXAST_RAW_FIN_TIMEOUT(const struct nx_action_fin_timeout *naft,
                             enum ofp_version ofp_version OVS_UNUSED,
                             struct ofpbuf *out)
{
    struct ofpact_fin_timeout *oft;

    oft = ofpact_put_FIN_TIMEOUT(out);
    oft->fin_idle_timeout = ntohs(naft->fin_idle_timeout);
    oft->fin_hard_timeout = ntohs(naft->fin_hard_timeout);
    return 0;
}