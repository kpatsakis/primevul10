create_backup (char const *to, struct stat *to_st, int *to_errno,
	       bool leave_original, bool remember_backup)
{
  struct stat tmp_st;
  int tmp_errno;

  /* When the input to patch modifies the same file more than once, patch only
     backs up the initial version of each file.

     To figure out which files have already been backed up, patch remembers the
     files that replace the original files.  Files not known already are backed
     up; files already known have already been backed up before, and are
     skipped.

     When a patch deletes a file, this leaves patch without such a "sentinel"
     file.  In that case, patch remembers the *backup file* instead; when a
     patch creates a file, patch checks if the *backup file* is already known.

     This strategy is not fully compatible with numbered backups: when a patch
     deletes and later recreates a file with numbered backups, two numbered
     backups will be created.  */

  if (! to_st || ! to_errno)
    {
      to_st = &tmp_st;
      to_errno = &tmp_errno;
    }
  *to_errno = lstat (to, to_st) == 0 ? 0 : errno;

  if (! to_errno && ! (S_ISREG (to_st->st_mode) || S_ISLNK (to_st->st_mode)))
    fatal ("File %s is not a %s -- refusing to create backup",
	   to, S_ISLNK (to_st->st_mode) ? "symbolic link" : "regular file");

  if (! *to_errno && file_already_seen (to_st))
    {
      if (debug & 4)
	say ("File %s already seen\n", quotearg (to));
    }
  else
    {
      int try_makedirs_errno = 0;
      char *bakname;

      if (origprae || origbase || origsuff)
	{
	  char const *p = origprae ? origprae : "";
	  char const *b = origbase ? origbase : "";
	  char const *s = origsuff ? origsuff : "";
	  char const *t = to;
	  size_t plen = strlen (p);
	  size_t blen = strlen (b);
	  size_t slen = strlen (s);
	  size_t tlen = strlen (t);
	  char const *o;
	  size_t olen;

	  for (o = t + tlen, olen = 0;
	       o > t && ! ISSLASH (*(o - 1));
	       o--)
	    /* do nothing */ ;
	  olen = t + tlen - o;
	  tlen -= olen;
	  bakname = xmalloc (plen + tlen + blen + olen + slen + 1);
	  memcpy (bakname, p, plen);
	  memcpy (bakname + plen, t, tlen);
	  memcpy (bakname + plen + tlen, b, blen);
	  memcpy (bakname + plen + tlen + blen, o, olen);
	  memcpy (bakname + plen + tlen + blen + olen, s, slen + 1);

	  if ((origprae
	       && (contains_slash (origprae + FILE_SYSTEM_PREFIX_LEN (origprae))
		   || contains_slash (to)))
	      || (origbase && contains_slash (origbase)))
	    try_makedirs_errno = ENOENT;
	}
      else
	{
	  bakname = find_backup_file_name (to, backup_type);
	  if (!bakname)
	    xalloc_die ();
	}

      if (*to_errno)
	{
	  struct stat backup_st;
	  int fd;

	  if (lstat (bakname, &backup_st) == 0
	      && file_already_seen (&backup_st))
	    {
	      if (debug & 4)
		say ("File %s already seen\n", quotearg (to));
	    }
	  else
	    {
	      if (debug & 4)
		say ("Creating empty file %s\n", quotearg (bakname));

	      try_makedirs_errno = ENOENT;
	      unlink (bakname);
	      while ((fd = creat (bakname, 0666)) < 0)
		{
		  if (errno != try_makedirs_errno)
		    pfatal ("Can't create file %s", quotearg (bakname));
		  makedirs (bakname);
		  try_makedirs_errno = 0;
		}
	      if (remember_backup && fstat (fd, &backup_st) == 0)
		insert_file (&backup_st);
	      if (close (fd) != 0)
		pfatal ("Can't close file %s", quotearg (bakname));
	    }
	}
      else if (leave_original)
	create_backup_copy (to, bakname, to_st, try_makedirs_errno == 0,
			    remember_backup);
      else
	{
	  if (debug & 4)
	    say ("Renaming file %s to %s\n",
		 quotearg_n (0, to), quotearg_n (1, bakname));
	  while (rename (to, bakname) != 0)
	    {
	      if (errno == try_makedirs_errno)
		{
		  makedirs (bakname);
		  try_makedirs_errno = 0;
		}
	      else if (errno == EXDEV)
		{
		  create_backup_copy (to, bakname, to_st,
				      try_makedirs_errno == 0,
				      remember_backup);
		  unlink (to);
		  break;
		}
	      else
		pfatal ("Can't rename file %s to %s",
			quotearg_n (0, to), quotearg_n (1, bakname));
	    }
	  if (remember_backup)
	    insert_file (to_st);
	}
      free (bakname);
    }
}