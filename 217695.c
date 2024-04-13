check_SET_IP_TTL(const struct ofpact_ip_ttl *a OVS_UNUSED,
                 struct ofpact_check_params *cp)
{
    return check_set_ip(cp);
}