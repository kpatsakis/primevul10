atchinfo_free (void *p)
{
  struct atchinfo *ap = p;
  free (ap->id);
  free (ap->encoding);
  free (ap->content_type);
  free (ap->name);
  free (ap->filename);
  mu_stream_destroy (&ap->source);
  free (ap);
}