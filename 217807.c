parse_DEC_MPLS_TTL(char *arg OVS_UNUSED, const struct ofpact_parse_params *pp)
{
    ofpact_put_DEC_MPLS_TTL(pp->ofpacts);
    return NULL;
}