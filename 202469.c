file_not_found (void)
{
  const char *error = 0;

  LT__GETERROR (error);
  if (error == LT__STRERROR (FILE_NOT_FOUND))
    return 1;

  return 0;
}