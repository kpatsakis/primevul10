glob_testdir (dir, flags)
     char *dir;
     int flags;
{
  struct stat finfo;
  int r;

/*itrace("glob_testdir: testing %s" flags = %d, dir, flags);*/
#if defined (HAVE_LSTAT)
  r = (flags & GX_ALLDIRS) ? lstat (dir, &finfo) : stat (dir, &finfo);
#else
  r = stat (dir, &finfo);
#endif
  if (r < 0)
    return (-1);

#if defined (S_ISLNK)
  if (S_ISLNK (finfo.st_mode))
    return (-2);
#endif

  if (S_ISDIR (finfo.st_mode) == 0)
    return (-1);

  return (0);
}