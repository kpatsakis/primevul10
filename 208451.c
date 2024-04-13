int was_set_insecurely(char_u *opt, int opt_flags)
{
  int idx = findoption(opt);

  if (idx >= 0) {
    uint32_t *flagp = insecure_flag(idx, opt_flags);
    return (*flagp & P_INSECURE) != 0;
  }
  EMSG2(_(e_intern2), "was_set_insecurely()");
  return -1;
}