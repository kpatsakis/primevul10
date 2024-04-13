test_for_canon_directory (name)
     const char *name;
{
  char *fn;
  int r;

  fn = (*name == '~') ? bash_tilde_expand (name, 0) : savestring (name);
  bash_filename_stat_hook (&fn);
  r = file_isdir (fn);
  free (fn);

  return (r);
}