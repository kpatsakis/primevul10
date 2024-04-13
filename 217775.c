check_SET_MPLS_TTL(const struct ofpact_mpls_ttl *a OVS_UNUSED,
                   struct ofpact_check_params *cp)
{
    return check_set_mpls(cp);
}