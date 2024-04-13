static void didset_options2(void)
{
  // Initialize the highlight_attr[] table.
  (void)highlight_changed();

  // Parse default for 'clipboard'.
  (void)opt_strings_flags(p_cb, p_cb_values, &cb_flags, true);

  // Parse default for 'fillchars'.
  (void)set_chars_option(&p_fcs);

  // Parse default for 'listchars'.
  (void)set_chars_option(&p_lcs);

  // Parse default for 'wildmode'.
  check_opt_wim();
}