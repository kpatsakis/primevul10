parse_SET_IP_ECN(char *arg, const struct ofpact_parse_params *pp)
{
    uint8_t ecn;
    char *error;

    error = str_to_u8(arg, "ECN", &ecn);
    if (error) {
        return error;
    }

    if (ecn & ~IP_ECN_MASK) {
        return xasprintf("%s: not a valid ECN", arg);
    }
    ofpact_put_SET_IP_ECN(pp->ofpacts)->ecn = ecn;
    return NULL;
}