ofpacts_parse_instructions(const char *s, const struct ofpact_parse_params *pp)
{
    return ofpacts_parse_copy(s, pp, true, 0);
}