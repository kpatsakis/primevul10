parse_SET_MPLS_TC(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_mpls_tc *mpls_tc = ofpact_put_SET_MPLS_TC(pp->ofpacts);

    if (*arg == '\0') {
        return xstrdup("set_mpls_tc: expected tc.");
    }

    mpls_tc->tc = atoi(arg);
    return NULL;
}