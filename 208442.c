static char_u *set_chars_option(char_u **varp)
{
  int round, i, len, entries;
  char_u      *p, *s;
  int c1, c2 = 0;
  struct charstab {
    int     *cp;
    char    *name;
  };
  static struct charstab filltab[] =
  {
    {&fill_stl,     "stl"},
    {&fill_stlnc,   "stlnc"},
    {&fill_vert,    "vert"},
    {&fill_fold,    "fold"},
    {&fill_diff,    "diff"},
  };
  static struct charstab lcstab[] =
  {
    {&lcs_eol,      "eol"},
    {&lcs_ext,      "extends"},
    {&lcs_nbsp,     "nbsp"},
    {&lcs_prec,     "precedes"},
    {&lcs_space,    "space"},
    {&lcs_tab2,     "tab"},
    {&lcs_trail,    "trail"},
    {&lcs_conceal,  "conceal"},
  };
  struct charstab *tab;

  if (varp == &p_lcs) {
    tab = lcstab;
    entries = ARRAY_SIZE(lcstab);
  } else {
    tab = filltab;
    entries = ARRAY_SIZE(filltab);
  }

  /* first round: check for valid value, second round: assign values */
  for (round = 0; round <= 1; ++round) {
    if (round > 0) {
      /* After checking that the value is valid: set defaults: space for
       * 'fillchars', NUL for 'listchars' */
      for (i = 0; i < entries; ++i)
        if (tab[i].cp != NULL)
          *(tab[i].cp) = (varp == &p_lcs ? NUL : ' ');
      if (varp == &p_lcs)
        lcs_tab1 = NUL;
      else
        fill_diff = '-';
    }
    p = *varp;
    while (*p) {
      for (i = 0; i < entries; ++i) {
        len = (int)STRLEN(tab[i].name);
        if (STRNCMP(p, tab[i].name, len) == 0
            && p[len] == ':'
            && p[len + 1] != NUL) {
          s = p + len + 1;
          c1 = mb_ptr2char_adv(&s);
          if (mb_char2cells(c1) > 1)
            continue;
          if (tab[i].cp == &lcs_tab2) {
            if (*s == NUL)
              continue;
            c2 = mb_ptr2char_adv(&s);
            if (mb_char2cells(c2) > 1)
              continue;
          }
          if (*s == ',' || *s == NUL) {
            if (round) {
              if (tab[i].cp == &lcs_tab2) {
                lcs_tab1 = c1;
                lcs_tab2 = c2;
              } else if (tab[i].cp != NULL)
                *(tab[i].cp) = c1;

            }
            p = s;
            break;
          }
        }
      }

      if (i == entries)
        return e_invarg;
      if (*p == ',')
        ++p;
    }
  }

  return NULL;          /* no error */
}