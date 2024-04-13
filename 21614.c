show_shell_usage (fp, extra)
     FILE *fp;
     int extra;
{
  int i;
  char *set_opts, *s, *t;

  if (extra)
    fprintf (fp, _("GNU bash, version %s-(%s)\n"), shell_version_string (), MACHTYPE);
  fprintf (fp, _("Usage:\t%s [GNU long option] [option] ...\n\t%s [GNU long option] [option] script-file ...\n"),
	     shell_name, shell_name);
  fputs (_("GNU long options:\n"), fp);
  for (i = 0; long_args[i].name; i++)
    fprintf (fp, "\t--%s\n", long_args[i].name);

  fputs (_("Shell options:\n"), fp);
  fputs (_("\t-ilrsD or -c command or -O shopt_option\t\t(invocation only)\n"), fp);

  for (i = 0, set_opts = 0; shell_builtins[i].name; i++)
    if (STREQ (shell_builtins[i].name, "set"))
      {
	set_opts = savestring (shell_builtins[i].short_doc);
	break;
      }

  if (set_opts)
    {
      s = strchr (set_opts, '[');
      if (s == 0)
	s = set_opts;
      while (*++s == '-')
	;
      t = strchr (s, ']');
      if (t)
	*t = '\0';
      fprintf (fp, _("\t-%s or -o option\n"), s);
      free (set_opts);
    }

  if (extra)
    {
      fprintf (fp, _("Type `%s -c \"help set\"' for more information about shell options.\n"), shell_name);
      fprintf (fp, _("Type `%s -c help' for more information about shell builtin commands.\n"), shell_name);
      fprintf (fp, _("Use the `bashbug' command to report bugs.\n"));
      fprintf (fp, "\n");
      fprintf (fp, _("bash home page: <http://www.gnu.org/software/bash>\n"));
      fprintf (fp, _("General help using GNU software: <http://www.gnu.org/gethelp/>\n"));
    }
}