void set_init_2(void)
{
  int idx;

  /*
   * 'scroll' defaults to half the window height. Note that this default is
   * wrong when the window height changes.
   */
  set_number_default("scroll", Rows / 2);
  idx = findoption((char_u *)"scroll");
  if (idx >= 0 && !(options[idx].flags & P_WAS_SET))
    set_option_default(idx, OPT_LOCAL, p_cp);
  comp_col();

  /*
   * 'window' is only for backwards compatibility with Vi.
   * Default is Rows - 1.
   */
  if (!option_was_set((char_u *)"window"))
    p_window = Rows - 1;
  set_number_default("window", Rows - 1);
  parse_shape_opt(SHAPE_CURSOR);   /* set cursor shapes from 'guicursor' */
  (void)parse_printoptions();       /* parse 'printoptions' default value */
}