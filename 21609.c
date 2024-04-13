exit_shell (s)
     int s;
{
  fflush (stdout);		/* XXX */
  fflush (stderr);

  /* Clean up the terminal if we are in a state where it's been modified. */
#if defined (READLINE)
  if (RL_ISSTATE (RL_STATE_TERMPREPPED) && rl_deprep_term_function)
    (*rl_deprep_term_function) ();
#endif
  if (read_tty_modified ())
    read_tty_cleanup ();

  /* Do trap[0] if defined.  Allow it to override the exit status
     passed to us. */
  if (signal_is_trapped (0))
    s = run_exit_trap ();

#if defined (PROCESS_SUBSTITUTION)
  unlink_fifo_list ();
#endif /* PROCESS_SUBSTITUTION */

#if defined (HISTORY)
  if (remember_on_history)
    maybe_save_shell_history ();
#endif /* HISTORY */

#if defined (COPROCESS_SUPPORT)
  coproc_flush ();
#endif

#if defined (JOB_CONTROL)
  /* If the user has run `shopt -s huponexit', hangup all jobs when we exit
     an interactive login shell.  ksh does this unconditionally. */
  if (interactive_shell && login_shell && hup_on_exit)
    hangup_all_jobs ();

  /* If this shell is interactive, or job control is active, terminate all
     stopped jobs and restore the original terminal process group.  Don't do
     this if we're in a subshell and calling exit_shell after, for example,
     a failed word expansion.  We want to do this even if the shell is not
     interactive because we set the terminal's process group when job control
     is enabled regardless of the interactive status. */
  if (subshell_environment == 0)
    end_job_control ();
#endif /* JOB_CONTROL */

  /* Always return the exit status of the last command to our parent. */
  sh_exit (s);
}