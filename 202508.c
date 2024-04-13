lt_dlsetsearchpath (const char *search_path)
{
  int   errors	    = 0;

  FREE (user_search_path);

  if (!search_path || !LT_STRLEN (search_path))
    {
      return errors;
    }

  if (canonicalize_path (search_path, &user_search_path) != 0)
    ++errors;

  return errors;
}