shell_glob_filename (pathname)
     const char *pathname;
{
#if defined (USE_POSIX_GLOB_LIBRARY)
  register int i;
  char *temp, **results;
  glob_t filenames;
  int glob_flags;

  temp = quote_string_for_globbing (pathname, QGLOB_FILENAME);

  filenames.gl_offs = 0;

#  if defined (GLOB_PERIOD)
  glob_flags = glob_dot_filenames ? GLOB_PERIOD : 0;
#  else
  glob_flags = 0;
#  endif /* !GLOB_PERIOD */

  glob_flags |= (GLOB_ERR | GLOB_DOOFFS);

  i = glob (temp, glob_flags, (posix_glob_errfunc_t *)NULL, &filenames);

  free (temp);

  if (i == GLOB_NOSPACE || i == GLOB_ABORTED)
    return ((char **)NULL);
  else if (i == GLOB_NOMATCH)
    filenames.gl_pathv = (char **)NULL;
  else if (i != 0)		/* other error codes not in POSIX.2 */
    filenames.gl_pathv = (char **)NULL;

  results = filenames.gl_pathv;

  if (results && ((GLOB_FAILED (results)) == 0))
    {
      if (should_ignore_glob_matches ())
	ignore_glob_matches (results);
      if (results && results[0])
	strvec_sort (results);
      else
	{
	  FREE (results);
	  results = (char **)NULL;
	}
    }

  return (results);

#else /* !USE_POSIX_GLOB_LIBRARY */

  char *temp, **results;
  int gflags, quoted_pattern;

  noglob_dot_filenames = glob_dot_filenames == 0;

  temp = quote_string_for_globbing (pathname, QGLOB_FILENAME);
  quoted_pattern = STREQ (pathname, temp) == 0;
  gflags = glob_star ? GX_GLOBSTAR : 0;
  results = glob_filename (temp, gflags);
  free (temp);

  if (results && ((GLOB_FAILED (results)) == 0))
    {
      if (should_ignore_glob_matches ())
	ignore_glob_matches (results);
      if (results && results[0])
	strvec_sort (results);
      else
	{
	  FREE (results);
	  results = (char **)&glob_error_return;
	}
    }

  return (results);
#endif /* !USE_POSIX_GLOB_LIBRARY */
}