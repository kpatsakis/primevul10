iri_new (void)
{
  struct iri *i = xmalloc (sizeof *i);
  i->uri_encoding = opt.encoding_remote ? xstrdup (opt.encoding_remote) : NULL;
  i->content_encoding = NULL;
  i->orig_url = NULL;
  i->utf8_encode = opt.enable_iri;
  return i;
}