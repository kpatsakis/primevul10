shell_initialize ()
{
  char hostname[256];
  int should_be_restricted;

  /* Line buffer output for stderr and stdout. */
  if (shell_initialized == 0)
    {
      sh_setlinebuf (stderr);
      sh_setlinebuf (stdout);
    }

  /* Sort the array of shell builtins so that the binary search in
     find_shell_builtin () works correctly. */
  initialize_shell_builtins ();

  /* Initialize the trap signal handlers before installing our own
     signal handlers.  traps.c:restore_original_signals () is responsible
     for restoring the original default signal handlers.  That function
     is called when we make a new child. */
  initialize_traps ();
  initialize_signals (0);

  /* It's highly unlikely that this will change. */
  if (current_host_name == 0)
    {
      /* Initialize current_host_name. */
      if (gethostname (hostname, 255) < 0)
	current_host_name = "??host??";
      else
	current_host_name = savestring (hostname);
    }

  /* Initialize the stuff in current_user that comes from the password
     file.  We don't need to do this right away if the shell is not
     interactive. */
  if (interactive_shell)
    get_current_user_info ();

  /* Initialize our interface to the tilde expander. */
  tilde_initialize ();

#if defined (RESTRICTED_SHELL)
  should_be_restricted = shell_is_restricted (shell_name);
#endif

  /* Initialize internal and environment variables.  Don't import shell
     functions from the environment if we are running in privileged or
     restricted mode or if the shell is running setuid. */
#if defined (RESTRICTED_SHELL)
  initialize_shell_variables (shell_environment, privileged_mode||restricted||should_be_restricted||running_setuid);
#else
  initialize_shell_variables (shell_environment, privileged_mode||running_setuid);
#endif

  /* Initialize the data structures for storing and running jobs. */
  initialize_job_control (jobs_m_flag);

  /* Initialize input streams to null. */
  initialize_bash_input ();

  initialize_flags ();

  /* Initialize the shell options.  Don't import the shell options
     from the environment variables $SHELLOPTS or $BASHOPTS if we are
     running in privileged or restricted mode or if the shell is running
     setuid. */
#if defined (RESTRICTED_SHELL)
  initialize_shell_options (privileged_mode||restricted||should_be_restricted||running_setuid);
  initialize_bashopts (privileged_mode||restricted||should_be_restricted||running_setuid);
#else
  initialize_shell_options (privileged_mode||running_setuid);
  initialize_bashopts (privileged_mode||running_setuid);
#endif
}