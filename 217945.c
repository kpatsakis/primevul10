ofpacts_parse_actions(const char *s, const struct ofpact_parse_params *pp)
{
    return ofpacts_parse_copy(s, pp, false, 0);
}