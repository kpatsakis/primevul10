find_file_callback (char *filename, void *data1, void *data2)
{
  char	     **pdir	= (char **) data1;
  FILE	     **pfile	= (FILE **) data2;
  int	     is_done	= 0;

  assert (filename && *filename);
  assert (pdir);
  assert (pfile);

  if ((*pfile = fopen (filename, LT_READTEXT_MODE)))
    {
      char *dirend = strrchr (filename, '/');

      if (dirend > filename)
	*dirend   = LT_EOS_CHAR;

      FREE (*pdir);
      *pdir   = lt__strdup (filename);
      is_done = (*pdir == 0) ? -1 : 1;
    }

  return is_done;
}