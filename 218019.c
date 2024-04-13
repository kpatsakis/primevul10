ofpacts_parse(char *str, const struct ofpact_parse_params *pp,
              bool allow_instructions, enum ofpact_type outer_action)
{
    if (pp->depth >= MAX_OFPACT_PARSE_DEPTH) {
        return xstrdup("Action nested too deeply");
    }
    CONST_CAST(struct ofpact_parse_params *, pp)->depth++;
    uint32_t orig_size = pp->ofpacts->size;
    char *error = ofpacts_parse__(str, pp, allow_instructions, outer_action);
    if (error) {
        pp->ofpacts->size = orig_size;
    }
    CONST_CAST(struct ofpact_parse_params *, pp)->depth--;
    return error;
}