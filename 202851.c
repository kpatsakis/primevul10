move_file (char const *from, int *from_needs_removal,
	   struct stat const *fromst,
	   char const *to, mode_t mode, bool backup)
{
  struct stat to_st;
  int to_errno = -1;

  if (backup)
    create_backup (to, &to_st, &to_errno, false, from == NULL);

  if (from)
    {
      if (S_ISLNK (mode))
	{
	  bool to_dir_known_to_exist = false;

	  /* FROM contains the contents of the symlink we have patched; need
	     to convert that back into a symlink. */
	  char *buffer = xmalloc (PATH_MAX);
	  int fd, size = 0, i;

	  if ((fd = open (from, O_RDONLY | O_BINARY)) < 0)
	    pfatal ("Can't reopen file %s", quotearg (from));
	  while ((i = read (fd, buffer + size, PATH_MAX - size)) > 0)
	    size += i;
	  if (i != 0 || close (fd) != 0)
	    read_fatal ();
	  buffer[size] = 0;

	  if (! backup)
	    {
	      if (unlink (to) == 0)
		to_dir_known_to_exist = true;
	    }
	  if (symlink (buffer, to) != 0)
	    {
	      if (errno == ENOENT && ! to_dir_known_to_exist)
		makedirs (to);
	      if (symlink (buffer, to) != 0)
		pfatal ("Can't create %s %s", "symbolic link", to);
	    }
	  free (buffer);
	  if (lstat (to, &to_st) != 0)
	    pfatal ("Can't get file attributes of %s %s", "symbolic link", to);
	  insert_file (&to_st);
	}
      else
	{
	  if (debug & 4)
	    say ("Renaming file %s to %s\n",
		 quotearg_n (0, from), quotearg_n (1, to));

	  if (rename (from, to) != 0)
	    {
	      bool to_dir_known_to_exist = false;

	      if (errno == ENOENT
		  && (to_errno == -1 || to_errno == ENOENT))
		{
		  makedirs (to);
		  to_dir_known_to_exist = true;
		  if (rename (from, to) == 0)
		    goto rename_succeeded;
		}

	      if (errno == EXDEV)
		{
		  struct stat tost;
		  if (! backup)
		    {
		      if (unlink (to) == 0)
			to_dir_known_to_exist = true;
		      else if (errno != ENOENT)
			pfatal ("Can't remove file %s", quotearg (to));
		    }
		  copy_file (from, to, &tost, 0, mode, to_dir_known_to_exist);
		  insert_file (&tost);
		  return;
		}

	      pfatal ("Can't rename file %s to %s",
		      quotearg_n (0, from), quotearg_n (1, to));
	    }

	rename_succeeded:
	  insert_file (fromst);
	  /* Do not clear *FROM_NEEDS_REMOVAL if it's possible that the
	     rename returned zero because FROM and TO are hard links to
	     the same file.  */
	  if (0 < to_errno
	      || (to_errno == 0 && to_st.st_nlink <= 1))
	    *from_needs_removal = 0;
	}
    }
  else if (! backup)
    {
      if (debug & 4)
	say ("Removing file %s\n", quotearg (to));
      if (unlink (to) != 0 && errno != ENOENT)
	pfatal ("Can't remove file %s", quotearg (to));
    }
}