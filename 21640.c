glob_name_is_acceptable (name)
     const char *name;
{
  struct ign *p;
  int flags;

  /* . and .. are never matched */
  if (name[0] == '.' && (name[1] == '\0' || (name[1] == '.' && name[2] == '\0')))
    return (0);

  flags = FNM_PATHNAME | FNMATCH_EXTFLAG | FNMATCH_NOCASEGLOB;
  for (p = globignore.ignores; p->val; p++)
    {
      if (strmatch (p->val, (char *)name, flags) != FNM_NOMATCH)
	return (0);
    }
  return (1);
}