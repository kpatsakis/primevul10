run_wgetrc (const char *file)
{
  FILE *fp;
  char *line = NULL;
  size_t bufsize = 0;
  int ln;
  int errcnt = 0;

  fp = fopen (file, "r");
  if (!fp)
    {
      fprintf (stderr, _("%s: Cannot read %s (%s).\n"), exec_name,
               file, strerror (errno));
      return true;                      /* not a fatal error */
    }
  ln = 1;
  while (getline (&line, &bufsize, fp) > 0)
    {
      char *com = NULL, *val = NULL;
      int comind;

      /* Parse the line.  */
      switch (parse_line (line, &com, &val, &comind))
        {
        case line_ok:
          /* If everything is OK, set the value.  */
          if (!setval_internal_tilde (comind, com, val))
            {
              fprintf (stderr, _("%s: Error in %s at line %d.\n"),
                       exec_name, file, ln);
              ++errcnt;
            }
          break;
        case line_syntax_error:
          fprintf (stderr, _("%s: Syntax error in %s at line %d.\n"),
                   exec_name, file, ln);
          ++errcnt;
          break;
        case line_unknown_command:
          fprintf (stderr, _("%s: Unknown command %s in %s at line %d.\n"),
                   exec_name, quote (com), file, ln);
          ++errcnt;
          break;
        case line_empty:
          break;
        default:
          abort ();
        }
      xfree_null (com);
      xfree_null (val);
      ++ln;
    }
  xfree (line);
  fclose (fp);

  return errcnt == 0;
}