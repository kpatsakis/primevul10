ofpacts_check_consistency(struct ofpact ofpacts[], size_t ofpacts_len,
                          enum ofputil_protocol needed_protocols,
                          struct ofpact_check_params *cp)
{
    enum ofperr error = ofpacts_check(ofpacts, ofpacts_len, cp);
    if (!error && needed_protocols & ~cp->usable_protocols) {
        return OFPERR_OFPBAC_MATCH_INCONSISTENT;
    }
    return error;
}