version_controller (char const *filename, bool readonly,
		    struct stat const *filestat, char **getbuf, char **diffbuf)
{
  struct stat cstat;
  char *dir = dir_name (filename);
  char *filebase = base_name (filename);
  char const *dotslash = *filename == '-' ? "./" : "";
  size_t dirlen = strlen (dir) + 1;
  size_t maxfixlen = sizeof "SCCS/" - 1 + sizeof SCCSPREFIX - 1;
  size_t maxtrysize = dirlen + strlen (filebase) + maxfixlen + 1;
  size_t quotelen = quote_system_arg (0, dir) + quote_system_arg (0, filebase);
  size_t maxgetsize = sizeof CLEARTOOL_CO + quotelen + maxfixlen;
  size_t maxdiffsize =
    (sizeof SCCSDIFF1 + sizeof SCCSDIFF2 + sizeof DEV_NULL - 1
     + 2 * quotelen + maxfixlen);
  char *trybuf = xmalloc (maxtrysize);
  char const *r = 0;

  sprintf (trybuf, "%s/", dir);

#define try1(f,a1)    (sprintf (trybuf + dirlen, f, a1),    stat (trybuf, &cstat) == 0)
#define try2(f,a1,a2) (sprintf (trybuf + dirlen, f, a1,a2), stat (trybuf, &cstat) == 0)

  /* Check that RCS file is not working file.
     Some hosts don't report file name length errors.  */

  if ((try2 ("RCS/%s%s", filebase, RCSSUFFIX)
       || try1 ("RCS/%s", filebase)
       || try2 ("%s%s", filebase, RCSSUFFIX))
      && ! (filestat
	    && filestat->st_dev == cstat.st_dev
	    && filestat->st_ino == cstat.st_ino))
    {
      if (getbuf)
	{
	  char *p = *getbuf = xmalloc (maxgetsize);
	  sprintf (p, readonly ? CHECKOUT : CHECKOUT_LOCKED, dotslash);
	  p += strlen (p);
	  p += quote_system_arg (p, filename);
	  *p = '\0';
	}

      if (diffbuf)
	{
	  char *p = *diffbuf = xmalloc (maxdiffsize);
	  sprintf (p, RCSDIFF1, dotslash);
	  p += strlen (p);
	  p += quote_system_arg (p, filename);
	  *p++ = '>';
	  strcpy (p, DEV_NULL);
	}

      r = "RCS";
    }
  else if (try2 ("SCCS/%s%s", SCCSPREFIX, filebase)
	   || try2 ("%s%s", SCCSPREFIX, filebase))
    {
      if (getbuf)
	{
	  char *p = *getbuf = xmalloc (maxgetsize);
	  sprintf (p, readonly ? GET : GET_LOCKED);
	  p += strlen (p);
	  p += quote_system_arg (p, trybuf);
	  *p = '\0';
	}

      if (diffbuf)
	{
	  char *p = *diffbuf = xmalloc (maxdiffsize);
	  strcpy (p, SCCSDIFF1);
	  p += sizeof SCCSDIFF1 - 1;
	  p += quote_system_arg (p, trybuf);
	  sprintf (p, SCCSDIFF2, dotslash);
	  p += strlen (p);
	  p += quote_system_arg (p, filename);
	  *p++ = '>';
	  strcpy (p, DEV_NULL);
	}

      r = "SCCS";
    }
  else if (!readonly && filestat
	   && try1 ("%s@@", filebase) && S_ISDIR (cstat.st_mode))
    {
      if (getbuf)
	{
	  char *p = *getbuf = xmalloc (maxgetsize);
	  strcpy (p, CLEARTOOL_CO);
	  p += sizeof CLEARTOOL_CO - 1;
	  p += quote_system_arg (p, filename);
	  *p = '\0';
	}

      if (diffbuf)
	*diffbuf = 0;

      r = "ClearCase";
     }
  else if (!readonly && filestat &&
           (getenv("P4PORT") || getenv("P4USER") || getenv("P4CONFIG")))
    {
      if (getbuf)
	{
	  char *p = *getbuf = xmalloc (maxgetsize);
	  strcpy (p, PERFORCE_CO);
	  p += sizeof PERFORCE_CO - 1;
	  p += quote_system_arg (p, filename);
	  *p = '\0';
	}

      if (diffbuf)
	*diffbuf = 0;

      r = "Perforce";
    }

  free (trybuf);
  free (filebase);
  free (dir);
  return r;
}