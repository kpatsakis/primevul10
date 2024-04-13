parse_SET_IP_DSCP(char *arg, const struct ofpact_parse_params *pp)

{
    uint8_t tos;
    char *error;

    error = str_to_u8(arg, "TOS", &tos);
    if (error) {
        return error;
    }

    if (tos & ~IP_DSCP_MASK) {
        return xasprintf("%s: not a valid TOS", arg);
    }
    ofpact_put_SET_IP_DSCP(pp->ofpacts)->dscp = tos;
    return NULL;
}