parse_SET_MPLS_LABEL(char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_mpls_label *mpls_label
        = ofpact_put_SET_MPLS_LABEL(pp->ofpacts);
    if (*arg == '\0') {
        return xstrdup("set_mpls_label: expected label.");
    }

    mpls_label->label = htonl(atoi(arg));
    return NULL;
}