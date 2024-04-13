free_urlpos (struct urlpos *l)
{
  while (l)
    {
      struct urlpos *next = l->next;
      if (l->url)
        url_free (l->url);
      xfree (l->local_name);
      xfree (l);
      l = next;
    }
}