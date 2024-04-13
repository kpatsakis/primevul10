lt_dlopenadvise (const char *filename, lt_dladvise advise)
{
  lt_dlhandle	handle	= 0;
  int		errors	= 0;
  const char *	saved_error	= 0;

  LT__GETERROR (saved_error);

  /* Can't have symbols hidden and visible at the same time!  */
  if (advise && advise->is_symlocal && advise->is_symglobal)
    {
      LT__SETERROR (CONFLICTING_FLAGS);
      return 0;
    }

  if (!filename
      || !advise
      || !advise->try_ext
      || has_library_ext (filename))
    {
      /* Just incase we missed a code path in try_dlopen() that reports
	 an error, but forgot to reset handle... */
      if (try_dlopen (&handle, filename, NULL, advise) != 0)
	return 0;

      return handle;
    }
  else if (filename && *filename)
    {

      /* First try appending ARCHIVE_EXT.  */
      errors += try_dlopen (&handle, filename, archive_ext, advise);

      /* If we found FILENAME, stop searching -- whether we were able to
	 load the file as a module or not.  If the file exists but loading
	 failed, it is better to return an error message here than to
	 report FILE_NOT_FOUND when the alternatives (foo.so etc) are not
	 in the module search path.  */
      if (handle || ((errors > 0) && !file_not_found ()))
	return handle;

#if defined(LT_MODULE_EXT)
      /* Try appending SHLIB_EXT.   */
      LT__SETERRORSTR (saved_error);
      errors = try_dlopen (&handle, filename, shlib_ext, advise);

      /* As before, if the file was found but loading failed, return now
	 with the current error message.  */
      if (handle || ((errors > 0) && !file_not_found ()))
	return handle;
#endif
    }

  /* Still here?  Then we really did fail to locate any of the file
     names we tried.  */
  LT__SETERROR (FILE_NOT_FOUND);
  return 0;
}