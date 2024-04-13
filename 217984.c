parse_STRIP_VLAN(char *arg OVS_UNUSED, const struct ofpact_parse_params *pp)
{
    ofpact_put_STRIP_VLAN(pp->ofpacts)->ofpact.raw = OFPAT_RAW10_STRIP_VLAN;
    return NULL;
}