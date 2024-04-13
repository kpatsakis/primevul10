name_is_valid (char const *name)
{
  char const *n;
  int i;
  bool is_valid = true;

  for (i = 0; i < ARRAY_SIZE (invalid_names); i++)
    {
      if (! invalid_names[i])
	break;
      if (! strcmp (invalid_names[i], name))
	return false;
    }

  if (IS_ABSOLUTE_FILE_NAME (name))
    is_valid = false;
  else
    for (n = name; *n; )
      {
	if (*n == '.' && *++n == '.' && ( ! *++n || ISSLASH (*n)))
	  {
	    is_valid = false;
	    break;
	  }
	while (*n && ! ISSLASH (*n))
	  n++;
	while (ISSLASH (*n))
	  n++;
      }

  /* Allow any filename if we are in the filesystem root.  */
  if (! is_valid && cwd_is_root (name))
    is_valid = true;

  if (! is_valid)
    {
      say ("Ignoring potentially dangerous file name %s\n", quotearg (name));
      if (i < ARRAY_SIZE (invalid_names))
	invalid_names[i] = name;
    }
  return is_valid;
}