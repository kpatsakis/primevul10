finddirs (pat, sdir, flags, ep, np)
     char *pat;
     char *sdir;
     int flags;
     struct globval **ep;
     int *np;
{
  char **r, *n;
  int ndirs;
  struct globval *ret, *e, *g;

/*itrace("finddirs: pat = `%s' sdir = `%s' flags = 0x%x", pat, sdir, flags);*/
  e = ret = 0;
  r = glob_vector (pat, sdir, flags);
  if (r == 0 || r[0] == 0)
    {
      if (np)
	*np = 0;
      if (ep)
        *ep = 0;
      if (r && r != &glob_error_return)
	free (r);
      return (struct globval *)0;
    }
  for (ndirs = 0; r[ndirs] != 0; ndirs++)
    {
      g = (struct globval *) malloc (sizeof (struct globval));
      if (g == 0)
	{
	  while (ret)		/* free list built so far */
	    {
	      g = ret->next;
	      free (ret);
	      ret = g;
	    }

	  free (r);
	  if (np)
	    *np = 0;
	  if (ep)
	    *ep = 0;
	  return (&finddirs_error_return);
	}
      if (e == 0)
	e = g;

      g->next = ret;
      ret = g;

      g->name = r[ndirs];
    }

  free (r);
  if (ep)
    *ep = e;
  if (np)
    *np = ndirs;

  return ret;
}