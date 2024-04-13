scan_linenum (char *s0, lin *linenum)
{
  char *s;
  lin n = 0;
  bool overflow = false;
  char numbuf[LINENUM_LENGTH_BOUND + 1];

  for (s = s0;  ISDIGIT (*s);  s++)
    {
      lin new_n = 10 * n + (*s - '0');
      overflow |= new_n / 10 != n;
      n = new_n;
    }

  if (s == s0)
    fatal ("missing line number at line %s: %s",
	   format_linenum (numbuf, p_input_line), buf);

  if (overflow)
    fatal ("line number %.*s is too large at line %s: %s",
	   (int) (s - s0), s0, format_linenum (numbuf, p_input_line), buf);

  *linenum = n;
  return s;
}