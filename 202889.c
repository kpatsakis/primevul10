make_tempfile (char const **name, char letter, char const *real_name,
	       int flags, mode_t mode)
{
  int try_makedirs_errno = ENOENT;
  char *template;

  if (real_name)
    {
      char *dirname, *basename;

      dirname = dir_name (real_name);
      basename = base_name (real_name);

      template = xmalloc (strlen (dirname) + 1 + strlen (basename) + 9);
      sprintf (template, "%s/%s.%cXXXXXX", dirname, basename, letter);
      free (dirname);
      free (basename);
    }
  else
    {
      char const *tmpdir;

      tmpdir = getenv ("TMPDIR");  /* Unix tradition */
      if (! tmpdir)
	tmpdir = getenv ("TMP");  /* DOS tradition */
      if (! tmpdir)
	tmpdir = getenv ("TEMP");  /* another DOS tradition */
      if (! tmpdir)
	tmpdir = TMPDIR;

      template = xmalloc (strlen (tmpdir) + 10);
      sprintf (template, "%s/p%cXXXXXX", tmpdir, letter);
    }
  for(;;)
    {
      int fd;

      if (gen_tempname (template, 0, flags, GT_NOCREATE))
        pfatal ("Can't create temporary file %s", template);
    retry:
      fd = open (template, O_CREAT | O_EXCL | flags, mode);
      if (fd == -1)
        {
	  if (errno == try_makedirs_errno)
	    {
	      makedirs (template);
	      /* FIXME: When patch fails, this may leave around empty
	         directories.  */
	      try_makedirs_errno = 0;
	      goto retry;
	    }
	  if (errno == EEXIST)
	    continue;
	  pfatal ("Can't create temporary file %s", template);
	}
      *name = template;
      return fd;
    }
}