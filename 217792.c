parse_NOTE(const char *arg, const struct ofpact_parse_params *pp)
{
    size_t start_ofs = pp->ofpacts->size;
    ofpact_put_NOTE(pp->ofpacts);
    arg = ofpbuf_put_hex(pp->ofpacts, arg, NULL);
    if (arg[0]) {
        return xstrdup("bad hex digit in `note' argument");
    }
    struct ofpact_note *note = ofpbuf_at_assert(pp->ofpacts, start_ofs,
                                                sizeof *note);
    note->length = pp->ofpacts->size - (start_ofs + sizeof *note);
    ofpact_finish_NOTE(pp->ofpacts, &note);
    return NULL;
}