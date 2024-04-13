parse_pop_vlan(const struct ofpact_parse_params *pp)
{
    ofpact_put_STRIP_VLAN(pp->ofpacts)->ofpact.raw = OFPAT_RAW11_POP_VLAN;
    return NULL;
}