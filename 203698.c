prefix_components (char *filename, bool checkdirs)
{
  int count = 0;
  struct stat stat_buf;
  int stat_result;
  char *f = filename + FILE_SYSTEM_PREFIX_LEN (filename);

  if (*f)
    while (*++f)
      if (ISSLASH (f[0]) && ! ISSLASH (f[-1]))
	{
	  if (checkdirs)
	    {
	      *f = '\0';
	      stat_result = stat (filename, &stat_buf);
	      *f = '/';
	      if (! (stat_result == 0 && S_ISDIR (stat_buf.st_mode)))
		break;
	    }

	  count++;
	}

  return count;
}