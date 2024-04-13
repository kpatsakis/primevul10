parse_REG_MOVE(const char *arg, const struct ofpact_parse_params *pp)
{
    struct ofpact_reg_move *move = ofpact_put_REG_MOVE(pp->ofpacts);
    return nxm_parse_reg_move(move, arg);
}