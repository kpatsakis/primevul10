check_SET_IP_ECN(const struct ofpact_ecn *a OVS_UNUSED,
                 struct ofpact_check_params *cp)
{
    return check_set_ip(cp);
}