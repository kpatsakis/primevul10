check_MULTIPATH(const struct ofpact_multipath *a,
                const struct ofpact_check_params *cp)
{
    return multipath_check(a, cp->match);
}