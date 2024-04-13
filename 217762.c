check_BUNDLE(const struct ofpact_bundle *a,
             const struct ofpact_check_params *cp)
{
    return bundle_check(a, cp->max_ports, cp->match);
}