char_u *get_highlight_default(void)
{
  int i;

  i = findoption((char_u *)"hl");
  if (i >= 0)
    return options[i].def_val[VI_DEFAULT];
  return (char_u *)NULL;
}