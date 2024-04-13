check_SET_MPLS_TC(const struct ofpact_mpls_tc *a OVS_UNUSED,
                  struct ofpact_check_params *cp)
{
    return check_set_mpls(cp);
}