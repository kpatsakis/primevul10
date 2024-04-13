savebuf (char const *s, size_t size)
{
  char *rv;

  if (! size)
    return NULL;

  rv = malloc (size);

  if (! rv)
    {
      if (! using_plan_a)
	xalloc_die ();
    }
  else
    memcpy (rv, s, size);

  return rv;
}