check_SET_MPLS_LABEL(const struct ofpact_mpls_label *a OVS_UNUSED,
                     struct ofpact_check_params *cp)
{
    return check_set_mpls(cp);
}