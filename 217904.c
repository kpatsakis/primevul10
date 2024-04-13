parse_CLEAR_ACTIONS(char *arg OVS_UNUSED, const struct ofpact_parse_params *pp)
{
    ofpact_put_CLEAR_ACTIONS(pp->ofpacts);
    return NULL;
}