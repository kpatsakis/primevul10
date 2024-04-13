lt_dlpath_insertdir (char **ppath, char *before, const char *dir)
{
  int    errors		= 0;
  char  *canonical	= 0;
  char  *argz		= 0;
  size_t argz_len	= 0;

  assert (ppath);
  assert (dir && *dir);

  if (canonicalize_path (dir, &canonical) != 0)
    {
      ++errors;
      goto cleanup;
    }

  assert (canonical && *canonical);

  /* If *PPATH is empty, set it to DIR.  */
  if (*ppath == 0)
    {
      assert (!before);		/* BEFORE cannot be set without PPATH.  */
      assert (dir);		/* Without DIR, don't call this function!  */

      *ppath = lt__strdup (dir);
      if (*ppath == 0)
	++errors;

      goto cleanup;
    }

  assert (ppath && *ppath);

  if (argzize_path (*ppath, &argz, &argz_len) != 0)
    {
      ++errors;
      goto cleanup;
    }

  /* Convert BEFORE into an equivalent offset into ARGZ.  This only works
     if *PPATH is already canonicalized, and hence does not change length
     with respect to ARGZ.  We canonicalize each entry as it is added to
     the search path, and don't call this function with (uncanonicalized)
     user paths, so this is a fair assumption.  */
  if (before)
    {
      assert (*ppath <= before);
      assert ((int) (before - *ppath) <= (int) strlen (*ppath));

      before = before - *ppath + argz;
    }

  if (lt_argz_insert (&argz, &argz_len, before, dir) != 0)
    {
      ++errors;
      goto cleanup;
    }

  argz_stringify (argz, argz_len, LT_PATHSEP_CHAR);
  MEMREASSIGN(*ppath, argz);

 cleanup:
  FREE (argz);
  FREE (canonical);

  return errors;
}