bash_progcomp_ignore_filenames (names)
     char **names;
{
  _ignore_completion_names (names, test_for_canon_directory);
  return 0;
}