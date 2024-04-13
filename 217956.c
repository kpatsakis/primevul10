parse_CT_CLEAR(char *arg OVS_UNUSED, const struct ofpact_parse_params *pp)
{
    ofpact_put_CT_CLEAR(pp->ofpacts);
    return NULL;
}