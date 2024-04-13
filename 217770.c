check_subactions(struct ofpact *ofpacts, size_t ofpacts_len,
                 struct ofpact_check_params *cp)
{
    struct ofpact_check_params sub = *cp;
    enum ofperr error = ofpacts_check(ofpacts, ofpacts_len, &sub);
    cp->usable_protocols &= sub.usable_protocols;
    return error;
}