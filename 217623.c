check_SET_IP_DSCP(const struct ofpact_dscp *a OVS_UNUSED,
                  struct ofpact_check_params *cp)
{
    return check_set_ip(cp);
}