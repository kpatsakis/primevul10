int findoption_len(const char_u *const arg, const size_t len)
{
  char *s, *p;
  static int quick_tab[27] = { 0, 0 };  // quick access table
  int is_term_opt;

  /*
   * For first call: Initialize the quick-access table.
   * It contains the index for the first option that starts with a certain
   * letter.  There are 26 letters, plus the first "t_" option.
   */
  if (quick_tab[1] == 0) {
    p = options[0].fullname;
    for (short int i = 1; (s = options[i].fullname) != NULL; i++) {
      if (s[0] != p[0]) {
        if (s[0] == 't' && s[1] == '_')
          quick_tab[26] = i;
        else
          quick_tab[CharOrdLow(s[0])] = i;
      }
      p = s;
    }
  }

  /*
   * Check for name starting with an illegal character.
   */
  if (len == 0 || arg[0] < 'a' || arg[0] > 'z') {
    return -1;
  }

  int opt_idx;
  is_term_opt = (len > 2 && arg[0] == 't' && arg[1] == '_');
  if (is_term_opt) {
    opt_idx = quick_tab[26];
  } else {
    opt_idx = quick_tab[CharOrdLow(arg[0])];
  }
  // Match full name
  for (; (s = options[opt_idx].fullname) != NULL; opt_idx++) {
    if (STRNCMP(arg, s, len) == 0 && s[len] == NUL) {
      break;
    }
  }
  if (s == NULL && !is_term_opt) {
    opt_idx = quick_tab[CharOrdLow(arg[0])];
    // Match short name
    for (; options[opt_idx].fullname != NULL; opt_idx++) {
      s = options[opt_idx].shortname;
      if (s != NULL && STRNCMP(arg, s, len) == 0 && s[len] == NUL) {
        break;
      }
      s = NULL;
    }
  }
  if (s == NULL)
    opt_idx = -1;
  return opt_idx;
}