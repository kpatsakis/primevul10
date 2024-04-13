check_REG_MOVE(const struct ofpact_reg_move *a,
               const struct ofpact_check_params *cp)
{
    return nxm_reg_move_check(a, cp->match);
}