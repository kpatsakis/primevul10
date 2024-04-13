init_noninteractive ()
{
#if defined (HISTORY)
  bash_history_reinit (0);
#endif /* HISTORY */
  interactive_shell = startup_state = interactive = 0;
  expand_aliases = posixly_correct;	/* XXX - was 0 not posixly_correct */
  no_line_editing = 1;
#if defined (JOB_CONTROL)
  /* Even if the shell is not interactive, enable job control if the -i or
     -m option is supplied at startup. */
  set_job_control (forced_interactive||jobs_m_flag);
#endif /* JOB_CONTROL */
}