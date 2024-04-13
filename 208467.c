void set_number_default(char *name, long val)
{
  int opt_idx;

  opt_idx = findoption((char_u *)name);
  if (opt_idx >= 0)
    options[opt_idx].def_val[VI_DEFAULT] = (char_u *)val;
}