run_command (const char *cmdopt)
{
  char *com, *val;
  int comind;
  switch (parse_line (cmdopt, &com, &val, &comind))
    {
    case line_ok:
      if (!setval_internal (comind, com, val))
        exit (WGET_EXIT_PARSE_ERROR);
      xfree (com);
      xfree (val);
      break;
    default:
      fprintf (stderr, _("%s: Invalid --execute command %s\n"),
               exec_name, quote (cmdopt));
      exit (WGET_EXIT_PARSE_ERROR);
    }
}