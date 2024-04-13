tartime (struct timespec t, bool full_time)
{
  enum { fraclen = sizeof ".FFFFFFFFF" - 1 };
  static char buffer[max (UINTMAX_STRSIZE_BOUND + 1,
			  INT_STRLEN_BOUND (int) + 16)
		     + fraclen];
  struct tm *tm;
  time_t s = t.tv_sec;
  int ns = t.tv_nsec;
  bool negative = s < 0;
  char *p;

  if (negative && ns != 0)
    {
      s++;
      ns = 1000000000 - ns;
    }

  tm = utc_option ? gmtime (&s) : localtime (&s);
  if (tm)
    {
      if (full_time)
	{
	  strftime (buffer, sizeof buffer, "%Y-%m-%d %H:%M:%S", tm);
	  code_ns_fraction (ns, buffer + strlen (buffer));
	}
      else
	strftime (buffer, sizeof buffer, "%Y-%m-%d %H:%M", tm);
      return buffer;
    }

  /* The time stamp cannot be broken down, most likely because it
     is out of range.  Convert it as an integer,
     right-adjusted in a field with the same width as the usual
     4-year ISO time format.  */
  p = umaxtostr (negative ? - (uintmax_t) s : s,
		 buffer + sizeof buffer - UINTMAX_STRSIZE_BOUND - fraclen);
  if (negative)
    *--p = '-';
  while ((buffer + sizeof buffer - sizeof "YYYY-MM-DD HH:MM"
	  + (full_time ? sizeof ":SS.FFFFFFFFF" - 1 : 0))
	 < p)
    *--p = ' ';
  if (full_time)
    code_ns_fraction (ns, buffer + sizeof buffer - 1 - fraclen);
  return p;
}