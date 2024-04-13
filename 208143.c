main_waitpid_check(pid_t pid)
{
  int status;
  int ret = 0;

  if (waitpid (pid, & status, 0) < 0)
    {
      ret = get_errno ();
      XPR(NT "external compression [pid %d] wait: %s\n",
	  pid, xd3_mainerror (ret));
    }
  else if (! WIFEXITED (status))
    {
      // SIGPIPE will be delivered to the child process whenever it
      // writes data after this process closes the pipe, 
      // happens if xdelta does not require access to the entire 
      // source file.  Considered normal.
      if (! WIFSIGNALED (status) || WTERMSIG (status) != SIGPIPE) 
	{
	  ret = ECHILD;
	  XPR(NT "external compression [pid %d] signal %d\n", pid, 
	      WIFSIGNALED (status) ? WTERMSIG (status) : WSTOPSIG (status));
	}
      else if (option_verbose)
	{
	  XPR(NT "external compression sigpipe\n");
	}
    }
  else if (WEXITSTATUS (status) != 0)
    {
      ret = ECHILD;
      if (option_verbose > 1)
	{
	  /* Presumably, the error was printed by the subprocess. */
	  XPR(NT "external compression [pid %d] exit %d\n",
	      pid, WEXITSTATUS (status));
	}
    }

  return ret;
}