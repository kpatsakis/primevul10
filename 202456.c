foreach_dirinpath (const char *search_path, const char *base_name,
		   foreach_callback_func *func, void *data1, void *data2)
{
  int	 result		= 0;
  size_t filenamesize	= 0;
  size_t lenbase	= LT_STRLEN (base_name);
  size_t argz_len	= 0;
  char *argz		= 0;
  char *filename	= 0;
  char *canonical	= 0;

  if (!search_path || !*search_path)
    {
      LT__SETERROR (FILE_NOT_FOUND);
      goto cleanup;
    }

  if (canonicalize_path (search_path, &canonical) != 0)
    goto cleanup;

  if (argzize_path (canonical, &argz, &argz_len) != 0)
    goto cleanup;

  {
    char *dir_name = 0;
    while ((dir_name = argz_next (argz, argz_len, dir_name)))
      {
	size_t lendir = LT_STRLEN (dir_name);

	if (1+ lendir + lenbase >= filenamesize)
	{
	  FREE (filename);
	  filenamesize	= 1+ lendir + 1+ lenbase; /* "/d" + '/' + "f" + '\0' */
	  filename	= MALLOC (char, filenamesize);
	  if (!filename)
	    goto cleanup;
	}

	assert (filenamesize > lendir);
	strcpy (filename, dir_name);

	if (base_name && *base_name)
	  {
	    if (filename[lendir -1] != '/')
	      filename[lendir++] = '/';
	    strcpy (filename +lendir, base_name);
	  }

	if ((result = (*func) (filename, data1, data2)))
	  {
	    break;
	  }
      }
  }

 cleanup:
  FREE (argz);
  FREE (canonical);
  FREE (filename);

  return result;
}