convert_dirent (const struct dirent *source)
{
  if (source == NULL)
    {
      struct readdir_result result = { NULL, };
      return result;
    }
  struct readdir_result result = READDIR_RESULT_INITIALIZER (source);
  return result;
}