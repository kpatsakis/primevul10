parse_METER(char *arg, const struct ofpact_parse_params *pp)
{
    *pp->usable_protocols &= OFPUTIL_P_OF13_UP;
    return str_to_u32(arg, &ofpact_put_METER(pp->ofpacts)->meter_id);
}