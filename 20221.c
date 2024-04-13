int gxht_dda_length(gx_dda_fixed *dda, int src_size)
{
    gx_dda_fixed d = (*dda);
    dda_advance(d, src_size);
    return abs(fixed2int_var_rounded(dda_current(d)) - fixed2int_var_rounded(dda_current(*dda)));
}