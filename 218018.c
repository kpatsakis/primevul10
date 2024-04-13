parse_BUNDLE(const char *arg, const struct ofpact_parse_params *pp)
{
    return bundle_parse(arg, pp->port_map, pp->ofpacts);
}