format_OUTPUT_REG(const struct ofpact_output_reg *a,
                  const struct ofpact_format_params *fp)
{
    ds_put_format(fp->s, "%soutput:%s", colors.special, colors.end);
    mf_format_subfield(&a->src, fp->s);
}