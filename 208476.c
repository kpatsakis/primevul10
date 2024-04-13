void reset_modifiable(void)
{
  int opt_idx;

  curbuf->b_p_ma = FALSE;
  p_ma = FALSE;
  opt_idx = findoption((char_u *)"ma");
  if (opt_idx >= 0)
    options[opt_idx].def_val[VI_DEFAULT] = FALSE;
}