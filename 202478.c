trim (char **dest, const char *str)
{
  /* remove the leading and trailing "'" from str
     and store the result in dest */
  const char *end   = strrchr (str, '\'');
  size_t len	    = LT_STRLEN (str);
  char *tmp;

  FREE (*dest);

  if (!end)
    return 1;

  if (len > 3 && str[0] == '\'')
    {
      tmp = MALLOC (char, end - str);
      if (!tmp)
	return 1;

      memcpy(tmp, &str[1], (end - str) - 1);
      tmp[(end - str) - 1] = LT_EOS_CHAR;
      *dest = tmp;
    }
  else
    {
      *dest = 0;
    }

  return 0;
}