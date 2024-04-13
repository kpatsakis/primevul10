glob_dir_to_array (dir, array, flags)
     char *dir, **array;
     int flags;
{
  register unsigned int i, l;
  int add_slash;
  char **result, *new;
  struct stat sb;

  l = strlen (dir);
  if (l == 0)
    {
      if (flags & GX_MARKDIRS)
	for (i = 0; array[i]; i++)
	  {
	    if ((stat (array[i], &sb) == 0) && S_ISDIR (sb.st_mode))
	      {
		l = strlen (array[i]);
		new = (char *)realloc (array[i], l + 2);
		if (new == 0)
		  return NULL;
		new[l] = '/';
		new[l+1] = '\0';
		array[i] = new;
	      }
	  }
      return (array);
    }

  add_slash = dir[l - 1] != '/';

  i = 0;
  while (array[i] != NULL)
    ++i;

  result = (char **) malloc ((i + 1) * sizeof (char *));
  if (result == NULL)
    return (NULL);

  for (i = 0; array[i] != NULL; i++)
    {
      /* 3 == 1 for NUL, 1 for slash at end of DIR, 1 for GX_MARKDIRS */
      result[i] = (char *) malloc (l + strlen (array[i]) + 3);

      if (result[i] == NULL)
	{
	  int ind;
	  for (ind = 0; ind < i; ind++)
	    free (result[ind]);
	  free (result);
	  return (NULL);
	}

      strcpy (result[i], dir);
      if (add_slash)
	result[i][l] = '/';
      if (array[i][0])
	{
	  strcpy (result[i] + l + add_slash, array[i]);
	  if (flags & GX_MARKDIRS)
	    {
	      if ((stat (result[i], &sb) == 0) && S_ISDIR (sb.st_mode))
		{
		  size_t rlen;
		  rlen = strlen (result[i]);
		  result[i][rlen] = '/';
		  result[i][rlen+1] = '\0';
		}
	    }
	}
      else
        result[i][l+add_slash] = '\0';
    }
  result[i] = NULL;

  /* Free the input array.  */
  for (i = 0; array[i] != NULL; i++)
    free (array[i]);
  free ((char *) array);

  return (result);
}