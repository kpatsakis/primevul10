quote_globbing_chars (string)
     const char *string;
{
  size_t slen;
  char *temp, *t;
  const char *s, *send;
  DECLARE_MBSTATE;

  slen = strlen (string);
  send = string + slen;

  temp = (char *)xmalloc (slen * 2 + 1);
  for (t = temp, s = string; *s; )
    {
      if (glob_char_p (s))
	*t++ = '\\';

      /* Copy a single (possibly multibyte) character from s to t,
	 incrementing both. */
      COPY_CHAR_P (t, s, send);
    }
  *t = '\0';
  return temp;
}