ofpacts_parse_copy(const char *s_, const struct ofpact_parse_params *pp,
                   bool allow_instructions, enum ofpact_type outer_action)
{
    char *error, *s;

    *pp->usable_protocols = OFPUTIL_P_ANY;

    s = xstrdup(s_);
    error = ofpacts_parse(s, pp, allow_instructions, outer_action);
    free(s);

    return error;
}