ftp_retrieve_glob (struct url *u, struct url *original_url,
                   ccon *con, int action)
{
  struct fileinfo *f, *start;
  uerr_t res;

  con->cmd |= LEAVE_PENDING;

  res = ftp_get_listing (u, original_url, con, &start);
  if (res != RETROK)
    return res;

  // Set the function used for glob matching.
  int (*matcher) (const char *, const char *, int)
    = opt.ignore_case ? fnmatch_nocase : fnmatch;

  // Set the function used to compare strings
#ifdef __VMS
  /* 2009-09-09 SMS.
   * Odd-ball compiler ("HP C V7.3-009 on OpenVMS Alpha V7.3-2")
   * bug causes spurious %CC-E-BADCONDIT complaint with this
   * "?:" statement.  (Different linkage attributes for strcmp()
   * and strcasecmp().)  Converting to "if" changes the
   * complaint to %CC-W-PTRMISMATCH on "cmp = strcmp;".  Adding
   * the senseless type cast clears the complaint, and looks
   * harmless.
   */
  int (*cmp) (const char *, const char *)
    = opt.ignore_case ? strcasecmp : (int (*)())strcmp;
#else /* def __VMS */
  int (*cmp) (const char *, const char *)
    = opt.ignore_case ? strcasecmp : strcmp;
#endif /* def __VMS [else] */

  f = start;
  while (f)
    {

      // Weed out files that do not confirm to the global rules given in
      // opt.accepts and opt.rejects
      if ((opt.accepts || opt.rejects) &&
          f->type != FT_DIRECTORY && !acceptable (f->name))
        {
          logprintf (LOG_VERBOSE, _("Rejecting %s.\n"),
                     quote (f->name));
          f = delelement (f, &start);
          continue;
        }


      // Identify and eliminate possibly harmful names or invalid entries.
      if (has_insecure_name_p (f->name) || is_invalid_entry (f))
        {
          logprintf (LOG_VERBOSE, _("Rejecting %s (Invalid Entry).\n"),
                     quote (f->name));
          f = delelement (f, &start);
          continue;
        }

      if (!accept_url (f->name))
        {
          logprintf (LOG_VERBOSE, _("%s is excluded/not-included through regex.\n"), f->name);
          f = delelement (f, &start);
          continue;
        }

      /* Now weed out the files that do not match our globbing pattern.
         If we are dealing with a globbing pattern, that is.  */
      if (*u->file)
        {
          if (action == GLOB_GLOBALL)
            {
              int matchres = matcher (u->file, f->name, 0);
              if (matchres == -1)
                {
                  logprintf (LOG_NOTQUIET, _("Error matching %s against %s: %s\n"),
                             u->file, quotearg_style (escape_quoting_style, f->name),
                             strerror (errno));
                  freefileinfo (start);
                  return RETRBADPATTERN;
                }
              if (matchres == FNM_NOMATCH)
                {
                  f = delelement (f, &start); /* delete the element from the list */
                  continue;
                }
            }
          else if (action == GLOB_GETONE)
            {
              if (0 != cmp(u->file, f->name))
                {
                  f = delelement (f, &start);
                  continue;
                }
            }
        }
      f = f->next;
    }

  /*
   * Now that preprocessing of the file listing is over, let's try to download
   * all the remaining files in our listing.
   */
  if (start)
    {
      /* Just get everything.  */
      res = ftp_retrieve_list (u, original_url, start, con);
    }
  else
    {
      if (action == GLOB_GLOBALL)
        {
          /* No luck.  */
          /* #### This message SUCKS.  We should see what was the
             reason that nothing was retrieved.  */
          logprintf (LOG_VERBOSE, _("No matches on pattern %s.\n"),
                     quote (u->file));
        }
      else if (action == GLOB_GETONE) /* GLOB_GETONE or GLOB_GETALL */
        {
          /* Let's try retrieving it anyway.  */
          con->st |= ON_YOUR_OWN;
          res = ftp_loop_internal (u, original_url, NULL, con, NULL, false);
          return res;
        }

      /* If action == GLOB_GETALL, and the file list is empty, there's
         no point in trying to download anything or in complaining about
         it.  (An empty directory should not cause complaints.)
      */
    }
  freefileinfo (start);
  if (opt.quota && total_downloaded_bytes > opt.quota)
    return QUOTEXC;
  else
    return res;
}