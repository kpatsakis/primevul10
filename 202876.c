parse_name (char const *s, int strip_leading, char const **endp)
{
  char *ret;

  while (ISSPACE ((unsigned char) *s))
    s++;
  if (*s == '"')
    ret = parse_c_string (s, endp);
  else
    {
      char const *t;

      for (t = s; *t && ! ISSPACE ((unsigned char) *t); t++)
	/* do nothing*/ ;
      ret = savebuf (s, t - s + 1);
      ret[t - s] = 0;
      if (endp)
	*endp = t;
    }
  if (! strip_leading_slashes (ret, strip_leading))
    {
      free (ret);
      ret = NULL;
    }
  return ret;
}