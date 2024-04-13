fatal_exit_handler (int sig)
{
  signal (sig, SIG_IGN);
  fatal_exit (sig);
}