check_METER(const struct ofpact_meter *a,
            const struct ofpact_check_params *cp OVS_UNUSED)
{
    uint32_t mid = a->meter_id;
    return mid == 0 || mid > OFPM13_MAX ? OFPERR_OFPMMFC_INVALID_METER : 0;
}