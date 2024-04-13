parse_MULTIPATH(const char *arg, const struct ofpact_parse_params *pp)
{
    return multipath_parse(ofpact_put_MULTIPATH(pp->ofpacts), arg);
}