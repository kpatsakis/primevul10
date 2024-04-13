parse_CLONE(char *arg, const struct ofpact_parse_params *pp)
{
    const size_t clone_offset = ofpacts_pull(pp->ofpacts);
    struct ofpact_nest *clone = ofpact_put_CLONE(pp->ofpacts);
    char *error;

    ofpbuf_pull(pp->ofpacts, sizeof *clone);
    error = ofpacts_parse_copy(arg, pp, false, 0);
    /* header points to the action list */
    pp->ofpacts->header = ofpbuf_push_uninit(pp->ofpacts, sizeof *clone);
    clone = pp->ofpacts->header;

    ofpact_finish_CLONE(pp->ofpacts, &clone);
    ofpbuf_push_uninit(pp->ofpacts, clone_offset);
    return error;
}