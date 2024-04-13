lt_dladdsearchdir (const char *search_dir)
{
  int errors = 0;

  if (search_dir && *search_dir)
    {
      if (lt_dlpath_insertdir (&user_search_path, 0, search_dir) != 0)
	++errors;
    }

  return errors;
}