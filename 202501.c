lt_dlinsertsearchdir (const char *before, const char *search_dir)
{
  int errors = 0;

  if (before)
    {
      if ((before < user_search_path)
	  || (before >= user_search_path + LT_STRLEN (user_search_path)))
	{
	  LT__SETERROR (INVALID_POSITION);
	  return 1;
	}
    }

  if (search_dir && *search_dir)
    {
      if (lt_dlpath_insertdir (&user_search_path,
			       (char *) before, search_dir) != 0)
	{
	  ++errors;
	}
    }

  return errors;
}