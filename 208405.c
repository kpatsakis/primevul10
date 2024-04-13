void comp_col(void)
{
  int last_has_status = (p_ls == 2 || (p_ls == 1 && firstwin != lastwin));

  sc_col = 0;
  ru_col = 0;
  if (p_ru) {
    ru_col = (ru_wid ? ru_wid : COL_RULER) + 1;
    /* no last status line, adjust sc_col */
    if (!last_has_status)
      sc_col = ru_col;
  }
  if (p_sc) {
    sc_col += SHOWCMD_COLS;
    if (!p_ru || last_has_status)           /* no need for separating space */
      ++sc_col;
  }
  assert(sc_col >= 0
         && INT_MIN + sc_col <= Columns
         && Columns - sc_col <= INT_MAX);
  sc_col = (int)(Columns - sc_col);
  assert(ru_col >= 0
         && INT_MIN + ru_col <= Columns
         && Columns - ru_col <= INT_MAX);
  ru_col = (int)(Columns - ru_col);
  if (sc_col <= 0)              /* screen too narrow, will become a mess */
    sc_col = 1;
  if (ru_col <= 0)
    ru_col = 1;
}