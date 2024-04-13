void check_options(void)
{
  int opt_idx;

  for (opt_idx = 0; options[opt_idx].fullname != NULL; opt_idx++)
    if ((options[opt_idx].flags & P_STRING) && options[opt_idx].var != NULL)
      check_string_option((char_u **)get_varp(&(options[opt_idx])));
}