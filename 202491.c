canonicalize_path (const char *path, char **pcanonical)
{
  char *canonical = 0;

  assert (path && *path);
  assert (pcanonical);

  canonical = MALLOC (char, 1+ LT_STRLEN (path));
  if (!canonical)
    return 1;

  {
    size_t dest = 0;
    size_t src;
    for (src = 0; path[src] != LT_EOS_CHAR; ++src)
      {
	/* Path separators are not copied to the beginning or end of
	   the destination, or if another separator would follow
	   immediately.  */
	if (path[src] == LT_PATHSEP_CHAR)
	  {
	    if ((dest == 0)
		|| (path[1+ src] == LT_PATHSEP_CHAR)
		|| (path[1+ src] == LT_EOS_CHAR))
	      continue;
	  }

	/* Anything other than a directory separator is copied verbatim.  */
	if ((path[src] != '/')
#if defined(LT_DIRSEP_CHAR)
	    && (path[src] != LT_DIRSEP_CHAR)
#endif
	    )
	  {
	    canonical[dest++] = path[src];
	  }
	/* Directory separators are converted and copied only if they are
	   not at the end of a path -- i.e. before a path separator or
	   NULL terminator.  */
	else if ((path[1+ src] != LT_PATHSEP_CHAR)
		 && (path[1+ src] != LT_EOS_CHAR)
#if defined(LT_DIRSEP_CHAR)
		 && (path[1+ src] != LT_DIRSEP_CHAR)
#endif
		 && (path[1+ src] != '/'))
	  {
	    canonical[dest++] = '/';
	  }
      }

    /* Add an end-of-string marker at the end.  */
    canonical[dest] = LT_EOS_CHAR;
  }

  /* Assign new value.  */
  *pcanonical = canonical;

  return 0;
}