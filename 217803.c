check_OUTPUT_REG(const struct ofpact_output_reg *a,
                 const struct ofpact_check_params *cp)
{
    return mf_check_src(&a->src, cp->match);
}