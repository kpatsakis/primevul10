check_DEC_MPLS_TTL(const struct ofpact_null *a OVS_UNUSED,
                   struct ofpact_check_params *cp)
{
    return check_set_mpls(cp);
}