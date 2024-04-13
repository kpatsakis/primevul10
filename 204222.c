remote_to_utf8 (const struct iri *iri, const char *str, char **new)
{
  bool ret = false;

  if (!iri->uri_encoding)
    return false;

  /* When `i->uri_encoding' == "UTF-8" there is nothing to convert.  But we must
     test for non-ASCII symbols for correct hostname processing in `idn_encode'
     function. */
  if (!c_strcasecmp (iri->uri_encoding, "UTF-8"))
    {
      const unsigned char *p;
      for (p = (unsigned char *) str; *p; p++)
        if (*p > 127)
          {
            *new = strdup (str);
            return true;
          }
      return false;
    }

  if (do_conversion ("UTF-8", iri->uri_encoding, str, strlen (str), new))
    ret = true;

  /* Test if something was converted */
  if (*new && !strcmp (str, *new))
    {
      xfree (*new);
      return false;
    }

  return ret;
}