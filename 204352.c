enforce_one_top_level (char **pfile_name)
{
  char *file_name = *pfile_name;
  char *p;

  for (p = file_name; *p && (ISSLASH (*p) || *p == '.'); p++)
    ;

  if (*p)
    {
      int pos = strlen (one_top_level_dir);
      if (strncmp (p, one_top_level_dir, pos) == 0)
	{
	  if (ISSLASH (p[pos]) || p[pos] == 0)
	    return;
	}

      *pfile_name = make_file_name (one_top_level_dir, file_name);
      normalize_filename_x (*pfile_name);
    }
  else
    *pfile_name = xstrdup (one_top_level_dir);
  free (file_name);
}