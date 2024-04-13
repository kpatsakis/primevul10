parse_PUSH_MPLS(char *arg, const struct ofpact_parse_params *pp)
{
    uint16_t ethertype;
    char *error;

    error = str_to_u16(arg, "push_mpls", &ethertype);
    if (!error) {
        ofpact_put_PUSH_MPLS(pp->ofpacts)->ethertype = htons(ethertype);
    }
    return error;
}