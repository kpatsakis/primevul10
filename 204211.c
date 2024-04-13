check_encoding_name (const char *encoding)
{
  const char *s = encoding;

  while (*s)
    {
      if (!c_isascii (*s) || c_isspace (*s))
        {
          logprintf (LOG_VERBOSE, _("Encoding %s isn't valid\n"), quote (encoding));
          return false;
        }

      s++;
    }

  return true;
}