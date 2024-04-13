list_files_by_dir (const char *dirnam, char **pargz, size_t *pargz_len)
{
  DIR	*dirp	  = 0;
  int    errors	  = 0;

  assert (dirnam && *dirnam);
  assert (pargz);
  assert (pargz_len);
  assert (dirnam[LT_STRLEN(dirnam) -1] != '/');

  dirp = opendir (dirnam);
  if (dirp)
    {
      struct dirent *dp	= 0;

      while ((dp = readdir (dirp)))
	if (dp->d_name[0] != '.')
	  if (lt_argz_insertdir (pargz, pargz_len, dirnam, dp))
	    {
	      ++errors;
	      break;
	    }

      closedir (dirp);
    }
  else
    ++errors;

  return errors;
}