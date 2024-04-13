static void init_signal_handling(void)
{
  struct sigaction sa;
  DBUG_ENTER("init_signal_handling");

#ifdef HAVE_STACKTRACE
  my_init_stacktrace();
#endif

  sa.sa_flags = SA_RESETHAND | SA_NODEFER;
  sigemptyset(&sa.sa_mask);
  sigprocmask(SIG_SETMASK, &sa.sa_mask, NULL);

  sa.sa_handler= signal_handler;

  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGABRT, &sa, NULL);
#ifdef SIGBUS
  sigaction(SIGBUS, &sa, NULL);
#endif
  sigaction(SIGILL, &sa, NULL);
  sigaction(SIGFPE, &sa, NULL);
  DBUG_VOID_RETURN;
}