set_options_default (
    int opt_flags                  /* OPT_FREE, OPT_LOCAL and/or OPT_GLOBAL */
)
{
  for (int i = 0; options[i].fullname; i++) {
    if (!(options[i].flags & P_NODEFAULT)) {
      set_option_default(i, opt_flags, p_cp);
    }
  }

  /* The 'scroll' option must be computed for all windows. */
  FOR_ALL_TAB_WINDOWS(tp, wp) {
    win_comp_scroll(wp);
  }

  parse_cino(curbuf);
}