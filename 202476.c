lt_dlgetsearchpath (void)
{
  const char *saved_path;

  saved_path = user_search_path;

  return saved_path;
}