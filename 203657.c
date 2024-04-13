setoptval (const char *com, const char *val, const char *optname)
{
  /* Prepend "--" to OPTNAME. */
  char *dd_optname = (char *) alloca (2 + strlen (optname) + 1);
  dd_optname[0] = '-';
  dd_optname[1] = '-';
  strcpy (dd_optname + 2, optname);

  assert (val != NULL);
  if (!setval_internal (command_by_name (com), dd_optname, val))
    exit (WGET_EXIT_PARSE_ERROR);
}