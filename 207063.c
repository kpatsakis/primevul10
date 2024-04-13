list_headers (void *item, void *data)
{
  struct add_header *hp = item;
  char *name = data;

  if (!name || strcmp (name, hp->name) == 0)
    {
      mu_printf ("%s: %s\n", hp->name, hp->value);
    }
  return 0;
}