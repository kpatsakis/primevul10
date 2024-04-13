did_set_option (
    int opt_idx,
    int opt_flags,              /* possibly with OPT_MODELINE */
    int new_value              /* value was replaced completely */
)
{
  options[opt_idx].flags |= P_WAS_SET;

  /* When an option is set in the sandbox, from a modeline or in secure mode
   * set the P_INSECURE flag.  Otherwise, if a new value is stored reset the
   * flag. */
  uint32_t *p = insecure_flag(opt_idx, opt_flags);
  if (secure
      || sandbox != 0
      || (opt_flags & OPT_MODELINE))
    *p = *p | P_INSECURE;
  else if (new_value)
    *p = *p & ~P_INSECURE;
}