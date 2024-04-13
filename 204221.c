parse_charset (const char *str)
{
  const char *end;
  char *charset;

  if (!str || !*str)
    return NULL;

  str = c_strcasestr (str, "charset=");
  if (!str)
    return NULL;

  str += 8;
  end = str;

  /* sXXXav: which chars should be banned ??? */
  while (*end && !c_isspace (*end))
    end++;

  /* sXXXav: could strdupdelim return NULL ? */
  charset = strdupdelim (str, end);

  /* Do a minimum check on the charset value */
  if (!check_encoding_name (charset))
    {
      xfree (charset);
      return NULL;
    }

  /*logprintf (LOG_VERBOSE, "parse_charset: %s\n", quote (charset));*/

  return charset;
}