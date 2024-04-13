tryall_dlopen_module (lt_dlhandle *handle, const char *prefix,
		      const char *dirname, const char *dlname,
		      lt_dladvise advise)
{
  int      error	= 0;
  char     *filename	= 0;
  size_t   filename_len	= 0;
  size_t   dirname_len	= LT_STRLEN (dirname);

  assert (handle);
  assert (dirname);
  assert (dlname);
#if defined(LT_DIRSEP_CHAR)
  /* Only canonicalized names (i.e. with DIRSEP chars already converted)
     should make it into this function:  */
  assert (strchr (dirname, LT_DIRSEP_CHAR) == 0);
#endif

  if (dirname_len > 0)
    if (dirname[dirname_len -1] == '/')
      --dirname_len;
  filename_len = dirname_len + 1 + LT_STRLEN (dlname);

  /* Allocate memory, and combine DIRNAME and MODULENAME into it.
     The PREFIX (if any) is handled below.  */
  filename  = MALLOC (char, filename_len + 1);
  if (!filename)
    return 1;

  sprintf (filename, "%.*s/%s", (int) dirname_len, dirname, dlname);

  /* Now that we have combined DIRNAME and MODULENAME, if there is
     also a PREFIX to contend with, simply recurse with the arguments
     shuffled.  Otherwise, attempt to open FILENAME as a module.  */
  if (prefix)
    {
      error += tryall_dlopen_module (handle, (const char *) 0,
				     prefix, filename, advise);
    }
  else if (tryall_dlopen (handle, filename, advise, 0) != 0)
    {
      ++error;
    }

  FREE (filename);
  return error;
}