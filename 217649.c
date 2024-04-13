parse_bundle_load(const char *arg, const struct ofpact_parse_params *pp)
{
    return bundle_parse_load(arg, pp->port_map, pp->ofpacts);
}