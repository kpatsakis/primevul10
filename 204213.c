set_uri_encoding (struct iri *i, const char *charset, bool force)
{
  DEBUGP (("URI encoding = %s\n", charset ? quote (charset) : "None"));
  if (!force && opt.encoding_remote)
    return;
  if (i->uri_encoding)
    {
      if (charset && !c_strcasecmp (i->uri_encoding, charset))
        return;
      xfree (i->uri_encoding);
    }

  i->uri_encoding = charset ? xstrdup (charset) : NULL;
}