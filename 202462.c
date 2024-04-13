find_file (const char *search_path, const char *base_name, char **pdir)
{
  FILE *file = 0;

  foreach_dirinpath (search_path, base_name, find_file_callback, pdir, &file);

  return file;
}