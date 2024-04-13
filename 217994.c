check_DEC_TTL(const struct ofpact_cnt_ids *a OVS_UNUSED,
              struct ofpact_check_params *cp)
{
    return check_set_ip(cp);
}