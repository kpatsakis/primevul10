initialize (void)
{
  char *file, *env_sysrc;
  bool ok = true;

  /* Run a non-standard system rc file when the according environment
     variable has been set. For internal testing purposes only!  */
  env_sysrc = getenv ("SYSTEM_WGETRC");
  if (env_sysrc && file_exists_p (env_sysrc))
    {
      ok &= run_wgetrc (env_sysrc);
      /* If there are any problems parsing the system wgetrc file, tell
         the user and exit */
      if (! ok)
        {
          fprintf (stderr, _("\
Parsing system wgetrc file (env SYSTEM_WGETRC) failed.  Please check\n\
'%s',\n\
or specify a different file using --config.\n"), env_sysrc);
          exit (WGET_EXIT_PARSE_ERROR);
        }
    }
  /* Otherwise, if SYSTEM_WGETRC is defined, use it.  */
#ifdef SYSTEM_WGETRC
  else if (file_exists_p (SYSTEM_WGETRC))
    ok &= run_wgetrc (SYSTEM_WGETRC);
  /* If there are any problems parsing the system wgetrc file, tell
     the user and exit */
  if (! ok)
    {
      fprintf (stderr, _("\
Parsing system wgetrc file failed.  Please check\n\
'%s',\n\
or specify a different file using --config.\n"), SYSTEM_WGETRC);
      exit (WGET_EXIT_PARSE_ERROR);
    }
#endif
  /* Override it with your own, if one exists.  */
  file = wgetrc_file_name ();
  if (!file)
    return;
  /* #### We should canonicalize `file' and SYSTEM_WGETRC with
     something like realpath() before comparing them with `strcmp'  */
#ifdef SYSTEM_WGETRC
  if (!strcmp (file, SYSTEM_WGETRC))
    {
      fprintf (stderr, _("\
%s: Warning: Both system and user wgetrc point to %s.\n"),
               exec_name, quote (file));
    }
  else
#endif
    ok &= run_wgetrc (file);

  /* If there were errors processing either `.wgetrc', abort. */
  if (!ok)
    exit (WGET_EXIT_PARSE_ERROR);

  xfree (file);
  return;
}