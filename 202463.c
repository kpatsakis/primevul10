load_deplibs (lt_dlhandle handle, char *deplibs)
{
  char	*p, *save_search_path = 0;
  int   depcount = 0;
  int	i;
  char	**names = 0;
  int	errors = 0;

  handle->depcount = 0;

  if (!deplibs)
    {
      return errors;
    }
  ++errors;

  if (user_search_path)
    {
      save_search_path = lt__strdup (user_search_path);
      if (!save_search_path)
	goto cleanup;
    }

  /* extract search paths and count deplibs */
  p = deplibs;
  while (*p)
    {
      if (!isspace ((unsigned char) *p))
	{
	  char *end = p+1;
	  while (*end && !isspace((unsigned char) *end))
	    {
	      ++end;
	    }

	  if (strncmp(p, "-L", 2) == 0 || strncmp(p, "-R", 2) == 0)
	    {
	      char save = *end;
	      *end = 0; /* set a temporary string terminator */
	      if (lt_dladdsearchdir(p+2))
		{
		  goto cleanup;
		}
	      *end = save;
	    }
	  else
	    {
	      ++depcount;
	    }

	  p = end;
	}
      else
	{
	  ++p;
	}
    }


  if (!depcount)
    {
      errors = 0;
      goto cleanup;
    }

  names = MALLOC (char *, depcount);
  if (!names)
    goto cleanup;

  /* now only extract the actual deplibs */
  depcount = 0;
  p = deplibs;
  while (*p)
    {
      if (isspace ((unsigned char) *p))
	{
	  ++p;
	}
      else
	{
	  char *end = p+1;
	  while (*end && !isspace ((unsigned char) *end))
	    {
	      ++end;
	    }

	  if (strncmp(p, "-L", 2) != 0 && strncmp(p, "-R", 2) != 0)
	    {
	      char *name;
	      char save = *end;
	      *end = 0; /* set a temporary string terminator */
	      if (strncmp(p, "-l", 2) == 0)
		{
		  size_t name_len = 3+ /* "lib" */ LT_STRLEN (p + 2);
		  name = MALLOC (char, 1+ name_len);
		  if (name)
		    sprintf (name, "lib%s", p+2);
		}
	      else
		name = lt__strdup(p);

	      if (!name)
		goto cleanup_names;

	      names[depcount++] = name;
	      *end = save;
	    }
	  p = end;
	}
    }

  /* load the deplibs (in reverse order)
     At this stage, don't worry if the deplibs do not load correctly,
     they may already be statically linked into the loading application
     for instance.  There will be a more enlightening error message
     later on if the loaded module cannot resolve all of its symbols.  */
  if (depcount)
    {
      lt_dlhandle cur = handle;
      int	j = 0;

      cur->deplibs = MALLOC (lt_dlhandle, depcount);
      if (!cur->deplibs)
	goto cleanup_names;

      for (i = 0; i < depcount; ++i)
	{
	  cur->deplibs[j] = lt_dlopenext(names[depcount-1-i]);
	  if (cur->deplibs[j])
	    {
	      ++j;
	    }
	}

      cur->depcount	= j;	/* Number of successfully loaded deplibs */
      errors		= 0;
    }

 cleanup_names:
  for (i = 0; i < depcount; ++i)
    {
      FREE (names[i]);
    }

 cleanup:
  FREE (names);
  /* restore the old search path */
  if (save_search_path) {
    MEMREASSIGN (user_search_path, save_search_path);
  }

  return errors;
}