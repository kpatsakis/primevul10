subshell_exit (s)
     int s;
{
  fflush (stdout);
  fflush (stderr);

  /* Do trap[0] if defined.  Allow it to override the exit status
     passed to us. */
  if (signal_is_trapped (0))
    s = run_exit_trap ();

  sh_exit (s);
}