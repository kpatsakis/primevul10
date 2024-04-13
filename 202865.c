version_get (char const *filename, char const *cs, bool exists, bool readonly,
	     char const *getbuf, struct stat *filestat)
{
  if (patch_get < 0)
    {
      ask ("Get file %s from %s%s? [y] ",
	   quotearg (filename), cs, readonly ? "" : " with lock");
      if (*buf == 'n')
	return 0;
    }

  if (dry_run)
    {
      if (! exists)
	fatal ("can't do dry run on nonexistent version-controlled file %s; invoke `%s' and try again",
	       quotearg (filename), getbuf);
    }
  else
    {
      if (verbosity == VERBOSE)
	say ("Getting file %s from %s%s...\n", quotearg (filename),
	     cs, readonly ? "" : " with lock");
      if (systemic (getbuf) != 0)
	fatal ("Can't get file %s from %s", quotearg (filename), cs);
      if (stat (filename, filestat) != 0)
	pfatal ("%s", quotearg (filename));
    }

  return 1;
}