format_REG_MOVE(const struct ofpact_reg_move *a,
                const struct ofpact_format_params *fp)
{
    nxm_format_reg_move(a, fp->s);
}