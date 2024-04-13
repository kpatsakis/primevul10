create_file (char const *file, int open_flags, mode_t mode,
	     bool to_dir_known_to_exist)
{
  int try_makedirs_errno = to_dir_known_to_exist ? 0 : ENOENT;
  int fd;
  mode |= S_IRUSR | S_IWUSR;
  mode &= ~ (S_IXUSR | S_IXGRP | S_IXOTH);
  do
    {
      if (! (O_CREAT && O_TRUNC))
	close (creat (file, mode));
      fd = open (file, O_CREAT | O_TRUNC | open_flags, mode);
      if (fd < 0)
	{
	  char *f;
	  if (errno != try_makedirs_errno)
	    pfatal ("Can't create file %s", quotearg (file));
	  f = xstrdup (file);
	  makedirs (f);
	  free (f);
	  try_makedirs_errno = 0;
	}
    } while (fd < 0);
  return fd;
}