iri_free (struct iri *i)
{
  if (i)
    {
      xfree (i->uri_encoding);
      xfree (i->content_encoding);
      xfree (i->orig_url);
      xfree (i);
    }
}