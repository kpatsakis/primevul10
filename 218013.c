parse_SET_MPLS_TTL(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_mpls_ttl *mpls_ttl = ofpact_put_SET_MPLS_TTL(pp->ofpacts);

    if (*arg == '\0') {
        return xstrdup("set_mpls_ttl: expected ttl.");
    }

    mpls_ttl->ttl = atoi(arg);
    return NULL;
}