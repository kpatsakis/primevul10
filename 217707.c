encode_FIN_TIMEOUT(const struct ofpact_fin_timeout *fin_timeout,
                   enum ofp_version ofp_version OVS_UNUSED,
                   struct ofpbuf *out)
{
    struct nx_action_fin_timeout *naft = put_NXAST_FIN_TIMEOUT(out);
    naft->fin_idle_timeout = htons(fin_timeout->fin_idle_timeout);
    naft->fin_hard_timeout = htons(fin_timeout->fin_hard_timeout);
}