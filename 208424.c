static bool briopt_check(win_T *wp)
{
  int bri_shift = 0;
  int bri_min = 20;
  bool bri_sbr = false;

  char_u *p = wp->w_p_briopt;
  while (*p != NUL)
  {
    if (STRNCMP(p, "shift:", 6) == 0
        && ((p[6] == '-' && ascii_isdigit(p[7])) || ascii_isdigit(p[6])))
    {
      p += 6;
      bri_shift = getdigits_int(&p);
    }
    else if (STRNCMP(p, "min:", 4) == 0 && ascii_isdigit(p[4]))
    {
      p += 4;
      bri_min = getdigits_int(&p);
    }
    else if (STRNCMP(p, "sbr", 3) == 0)
    {
      p += 3;
      bri_sbr = true;
    }
    if (*p != ',' && *p != NUL)
      return false;
    if (*p == ',')
      ++p;
  }

  wp->w_p_brishift = bri_shift;
  wp->w_p_brimin   = bri_min;
  wp->w_p_brisbr   = bri_sbr;

  return true;
}