glob_filename (pathname, flags)
     char *pathname;
     int flags;
{
  char **result, **new_result;
  unsigned int result_size;
  char *directory_name, *filename, *dname, *fn;
  unsigned int directory_len;
  int free_dirname;			/* flag */
  int dflags, hasglob;

  result = (char **) malloc (sizeof (char *));
  result_size = 1;
  if (result == NULL)
    return (NULL);

  result[0] = NULL;

  directory_name = NULL;

  /* Find the filename.  */
  filename = strrchr (pathname, '/');
#if defined (EXTENDED_GLOB)
  if (filename && extended_glob)
    {
      fn = glob_dirscan (pathname, '/');
#if DEBUG_MATCHING
      if (fn != filename)
	fprintf (stderr, "glob_filename: glob_dirscan: fn (%s) != filename (%s)\n", fn ? fn : "(null)", filename);
#endif
      filename = fn;
    }
#endif

  if (filename == NULL)
    {
      filename = pathname;
      directory_name = "";
      directory_len = 0;
      free_dirname = 0;
    }
  else
    {
      directory_len = (filename - pathname) + 1;
      directory_name = (char *) malloc (directory_len + 1);

      if (directory_name == 0)		/* allocation failed? */
	{
	  free (result);
	  return (NULL);
	}

      bcopy (pathname, directory_name, directory_len);
      directory_name[directory_len] = '\0';
      ++filename;
      free_dirname = 1;
    }

  hasglob = 0;
  /* If directory_name contains globbing characters, then we
     have to expand the previous levels.  Just recurse.
     If glob_pattern_p returns != [0,1] we have a pattern that has backslash
     quotes but no unquoted glob pattern characters. We dequote it below. */
  if (directory_len > 0 && (hasglob = glob_pattern_p (directory_name)) == 1)
    {
      char **directories, *d, *p;
      register unsigned int i;
      int all_starstar, last_starstar;

      all_starstar = last_starstar = 0;
      d = directory_name;
      dflags = flags & ~GX_MARKDIRS;
      /* Collapse a sequence of ** patterns separated by one or more slashes
	 to a single ** terminated by a slash or NUL */
      if ((flags & GX_GLOBSTAR) && d[0] == '*' && d[1] == '*' && (d[2] == '/' || d[2] == '\0'))
	{
	  p = d;
	  while (d[0] == '*' && d[1] == '*' && (d[2] == '/' || d[2] == '\0'))
	    {
	      p = d;
	      if (d[2])
		{
		  d += 3;
		  while (*d == '/')
		    d++;
		  if (*d == 0)
		    break;
		}
	    }
	  if (*d == 0)
	    all_starstar = 1;
	  d = p;
	  dflags |= GX_ALLDIRS|GX_ADDCURDIR;
	  directory_len = strlen (d);
	}

      /* If there is a non [star][star]/ component in directory_name, we
	 still need to collapse trailing sequences of [star][star]/ into
	 a single one and note that the directory name ends with [star][star],
	 so we can compensate if filename is [star][star] */
      if ((flags & GX_GLOBSTAR) && all_starstar == 0)
	{
	  int dl, prev;
	  prev = dl = directory_len;
	  while (dl >= 4 && d[dl - 1] == '/' &&
			   d[dl - 2] == '*' &&
			   d[dl - 3] == '*' &&
			   d[dl - 4] == '/')
	    prev = dl, dl -= 3;
	  if (dl != directory_len)
	    last_starstar = 1;
	  directory_len = prev;
	}

      /* If the directory name ends in [star][star]/ but the filename is
	 [star][star], just remove the final [star][star] from the directory
	 so we don't have to scan everything twice. */
      if (last_starstar && directory_len > 4 &&
	    filename[0] == '*' && filename[1] == '*' && filename[2] == 0)
	{
	  directory_len -= 3;
	}

      if (d[directory_len - 1] == '/')
	d[directory_len - 1] = '\0';

      directories = glob_filename (d, dflags|GX_RECURSE);

      if (free_dirname)
	{
	  free (directory_name);
	  directory_name = NULL;
	}

      if (directories == NULL)
	goto memory_error;
      else if (directories == (char **)&glob_error_return)
	{
	  free ((char *) result);
	  return ((char **) &glob_error_return);
	}
      else if (*directories == NULL)
	{
	  free ((char *) directories);
	  free ((char *) result);
	  return ((char **) &glob_error_return);
	}

      /* If we have something like [star][star]/[star][star], it's no use to
         glob **, then do it again, and throw half the results away.  */
      if (all_starstar && filename[0] == '*' && filename[1] == '*' && filename[2] == 0)
	{
	  free ((char *) directories);
	  free (directory_name);
	  directory_name = NULL;
	  directory_len = 0;
	  goto only_filename;
	}

      /* We have successfully globbed the preceding directory name.
	 For each name in DIRECTORIES, call glob_vector on it and
	 FILENAME.  Concatenate the results together.  */
      for (i = 0; directories[i] != NULL; ++i)
	{
	  char **temp_results;
	  int shouldbreak;

	  shouldbreak = 0;
	  /* XXX -- we've recursively scanned any directories resulting from
	     a `**', so turn off the flag.  We turn it on again below if
	     filename is `**' */
	  /* Scan directory even on a NULL filename.  That way, `*h/'
	     returns only directories ending in `h', instead of all
	     files ending in `h' with a `/' appended. */
	  dname = directories[i];
	  dflags = flags & ~(GX_MARKDIRS|GX_ALLDIRS|GX_ADDCURDIR);
	  /* last_starstar? */
	  if ((flags & GX_GLOBSTAR) && filename[0] == '*' && filename[1] == '*' && filename[2] == '\0')
	    dflags |= GX_ALLDIRS|GX_ADDCURDIR;
	  if (dname[0] == '\0' && filename[0])
	    {
	      dflags |= GX_NULLDIR;
	      dname = ".";	/* treat null directory name and non-null filename as current directory */
	    }

	  /* Special handling for symlinks to directories with globstar on */
	  if (all_starstar && (dflags & GX_NULLDIR) == 0)
	    {
	      int dlen;

	      /* If we have a directory name that is not null (GX_NULLDIR above)
		 and is a symlink to a directory, we return the symlink if
		 we're not `descending' into it (filename[0] == 0) and return
		 glob_error_return (which causes the code below to skip the
		 name) otherwise. I should fold this into a test that does both
		 checks instead of calling stat twice. */
	      if (glob_testdir (dname, flags|GX_ALLDIRS) == -2 && glob_testdir (dname, 0) == 0)
		{
		  if (filename[0] != 0)
		    temp_results = (char **)&glob_error_return;		/* skip */
		  else
		    {
		      /* Construct array to pass to glob_dir_to_array */
		      temp_results = (char **)malloc (2 * sizeof (char *));
		      if (temp_results == NULL)
			goto memory_error;
		      temp_results[0] = (char *)malloc (1);
		      if (temp_results[0] == 0)
			{
			  free (temp_results);
			  goto memory_error;
			}
		      **temp_results = '\0';
		      temp_results[1] = NULL;
		      dflags |= GX_SYMLINK;	/* mostly for debugging */
		    }
		}
	      else
		temp_results = glob_vector (filename, dname, dflags);
	    }
	  else
	    temp_results = glob_vector (filename, dname, dflags);

	  /* Handle error cases. */
	  if (temp_results == NULL)
	    goto memory_error;
	  else if (temp_results == (char **)&glob_error_return)
	    /* This filename is probably not a directory.  Ignore it.  */
	    ;
	  else
	    {
	      char **array;
	      register unsigned int l;

	      /* If we're expanding **, we don't need to glue the directory
		 name to the results; we've already done it in glob_vector */
	      if ((dflags & GX_ALLDIRS) && filename[0] == '*' && filename[1] == '*' && (filename[2] == '\0' || filename[2] == '/'))
		{
		  /* When do we remove null elements from temp_results?  And
		     how to avoid duplicate elements in the final result? */
		  /* If (dflags & GX_NULLDIR) glob_filename potentially left a
		     NULL placeholder in the temp results just in case
		     glob_vector/glob_dir_to_array did something with it, but
		     if it didn't, and we're not supposed to be passing them
		     through for some reason ((flags & GX_NULLDIR) == 0) we
		     need to remove all the NULL elements from the beginning
		     of TEMP_RESULTS. */
		  /* If we have a null directory name and ** as the filename,
		     we have just searched for everything from the current
		     directory on down. Break now (shouldbreak = 1) to avoid
		     duplicate entries in the final result. */
#define NULL_PLACEHOLDER(x)	((x) && *(x) && **(x) == 0)
		  if ((dflags & GX_NULLDIR) && (flags & GX_NULLDIR) == 0 &&
			NULL_PLACEHOLDER (temp_results))
#undef NULL_PLACEHOLDER
		    {
		      register int i, n;
		      for (n = 0; temp_results[n] && *temp_results[n] == 0; n++)
			;
		      i = n;
		      do
			temp_results[i - n] = temp_results[i];
		      while (temp_results[i++] != 0);
		      array = temp_results;
		      shouldbreak = 1;
		    }
	          else
		    array = temp_results;
		}
	      else if (dflags & GX_SYMLINK)
		array = glob_dir_to_array (directories[i], temp_results, flags);
	      else
		array = glob_dir_to_array (directories[i], temp_results, flags);
	      l = 0;
	      while (array[l] != NULL)
		++l;

	      new_result = (char **)realloc (result, (result_size + l) * sizeof (char *));

	      if (new_result == NULL)
		{
		  for (l = 0; array[l]; ++l)
		    free (array[l]);
		  free ((char *)array);
		  goto memory_error;
		}
	      result = new_result;

	      for (l = 0; array[l] != NULL; ++l)
		result[result_size++ - 1] = array[l];

	      result[result_size - 1] = NULL;

	      /* Note that the elements of ARRAY are not freed.  */
	      if (array != temp_results)
		free ((char *) array);
	      else if ((dflags & GX_ALLDIRS) && filename[0] == '*' && filename[1] == '*' && filename[2] == '\0')
		free (temp_results);	/* expanding ** case above */

	      if (shouldbreak)
		break;
	    }
	}
      /* Free the directories.  */
      for (i = 0; directories[i]; i++)
	free (directories[i]);

      free ((char *) directories);

      return (result);
    }

only_filename:
  /* If there is only a directory name, return it. */
  if (*filename == '\0')
    {
      result = (char **) realloc ((char *) result, 2 * sizeof (char *));
      if (result == NULL)
	{
	  if (free_dirname)
	    free (directory_name);
	  return (NULL);
	}
      /* If we have a directory name with quoted characters, and we are
	 being called recursively to glob the directory portion of a pathname,
	 we need to dequote the directory name before returning it so the
	 caller can read the directory */
      if (directory_len > 0 && hasglob == 2 && (flags & GX_RECURSE) != 0)
	{
	  dequote_pathname (directory_name);
	  directory_len = strlen (directory_name);
	}

      /* We could check whether or not the dequoted directory_name is a
	 directory and return it here, returning the original directory_name
	 if not, but we don't do that. We do return the dequoted directory
	 name if we're not being called recursively and the dequoted name
	 corresponds to an actual directory. For better backwards compatibility,
	 we can return &glob_error_return unconditionally in this case. */

      if (directory_len > 0 && hasglob == 2 && (flags & GX_RECURSE) == 0)
	{
#if 1
	  dequote_pathname (directory_name);
	  if (glob_testdir (directory_name, 0) < 0)
	    {
	      if (free_dirname)
		free (directory_name);
	      return ((char **)&glob_error_return);
	    }
#else
	  return ((char **)&glob_error_return);
#endif
	}

      /* Handle GX_MARKDIRS here. */
      result[0] = (char *) malloc (directory_len + 1);
      if (result[0] == NULL)
	goto memory_error;
      bcopy (directory_name, result[0], directory_len + 1);
      if (free_dirname)
	free (directory_name);
      result[1] = NULL;
      return (result);
    }
  else
    {
      char **temp_results;

      /* There are no unquoted globbing characters in DIRECTORY_NAME.
	 Dequote it before we try to open the directory since there may
	 be quoted globbing characters which should be treated verbatim. */
      if (directory_len > 0)
	dequote_pathname (directory_name);

      /* We allocated a small array called RESULT, which we won't be using.
	 Free that memory now. */
      free (result);

      /* Just return what glob_vector () returns appended to the
	 directory name. */
      /* If flags & GX_ALLDIRS, we're called recursively */
      dflags = flags & ~GX_MARKDIRS;
      if (directory_len == 0)
	dflags |= GX_NULLDIR;
      if ((flags & GX_GLOBSTAR) && filename[0] == '*' && filename[1] == '*' && filename[2] == '\0')
	{
	  dflags |= GX_ALLDIRS|GX_ADDCURDIR;
#if 0
	  /* If we want all directories (dflags & GX_ALLDIRS) and we're not
	     being called recursively as something like `echo [star][star]/[star].o'
	     ((flags & GX_ALLDIRS) == 0), we want to prevent glob_vector from
	     adding a null directory name to the front of the temp_results
	     array.  We turn off ADDCURDIR if not called recursively and
	     dlen == 0 */
#endif
	  if (directory_len == 0 && (flags & GX_ALLDIRS) == 0)
	    dflags &= ~GX_ADDCURDIR;
	}
      temp_results = glob_vector (filename,
				  (directory_len == 0 ? "." : directory_name),
				  dflags);

      if (temp_results == NULL || temp_results == (char **)&glob_error_return)
	{
	  if (free_dirname)
	    free (directory_name);
	  QUIT;			/* XXX - shell */
	  run_pending_traps ();
	  return (temp_results);
	}

      result = glob_dir_to_array ((dflags & GX_ALLDIRS) ? "" : directory_name, temp_results, flags);

      if (free_dirname)
	free (directory_name);
      return (result);
    }

  /* We get to memory_error if the program has run out of memory, or
     if this is the shell, and we have been interrupted. */
 memory_error:
  if (result != NULL)
    {
      register unsigned int i;
      for (i = 0; result[i] != NULL; ++i)
	free (result[i]);
      free ((char *) result);
    }

  if (free_dirname && directory_name)
    free (directory_name);

  QUIT;
  run_pending_traps ();

  return (NULL);
}