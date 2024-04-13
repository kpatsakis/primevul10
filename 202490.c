lt_dlopen (const char *filename)
{
  return lt_dlopenadvise (filename, NULL);
}