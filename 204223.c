set_content_encoding (struct iri *i, const char *charset)
{
  DEBUGP (("URI content encoding = %s\n", charset ? quote (charset) : "None"));
  if (opt.encoding_remote)
    return;
  if (i->content_encoding)
    {
      if (charset && !c_strcasecmp (i->content_encoding, charset))
        return;
      xfree (i->content_encoding);
    }

  i->content_encoding = charset ? xstrdup (charset) : NULL;
}