parse_WRITE_ACTIONS(char *arg, const struct ofpact_parse_params *pp)
{
    size_t ofs = ofpacts_pull(pp->ofpacts);
    struct ofpact_nest *on;
    char *error;

    /* Add a Write-Actions instruction and then pull it off. */
    ofpact_put(pp->ofpacts, OFPACT_WRITE_ACTIONS, sizeof *on);
    ofpbuf_pull(pp->ofpacts, sizeof *on);

    /* Parse nested actions.
     *
     * We pulled off "write-actions" and the previous actions because the
     * OFPACT_WRITE_ACTIONS is only partially constructed: its length is such
     * that it doesn't actually include the nested actions.  That means that
     * ofpacts_parse() would reject them as being part of an Apply-Actions that
     * follows a Write-Actions, which is an invalid order.  */
    error = ofpacts_parse(arg, pp, false, OFPACT_WRITE_ACTIONS);

    /* Put the Write-Actions back on and update its length. */
    on = ofpbuf_push_uninit(pp->ofpacts, sizeof *on);
    on->ofpact.len = pp->ofpacts->size;

    /* Put any previous actions or instructions back on. */
    ofpbuf_push_uninit(pp->ofpacts, ofs);

    return error;
}