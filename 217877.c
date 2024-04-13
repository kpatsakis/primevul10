parse_POP_QUEUE(const char *arg OVS_UNUSED,
                const struct ofpact_parse_params *pp)
{
    ofpact_put_POP_QUEUE(pp->ofpacts);
    return NULL;
}