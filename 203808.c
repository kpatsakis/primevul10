ftp_expected_bytes (const char *s)
{
  wgint res;

  while (1)
    {
      while (*s && *s != '(')
        ++s;
      if (!*s)
        return 0;
      ++s;                      /* skip the '(' */
      res = str_to_wgint (s, (char **) &s, 10);
      if (!*s)
        return 0;
      while (*s && c_isspace (*s))
        ++s;
      if (!*s)
        return 0;
      if (c_tolower (*s) != 'b')
        continue;
      if (c_strncasecmp (s, "byte", 4))
        continue;
      else
        break;
    }
  return res;
}