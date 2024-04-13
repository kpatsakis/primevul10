shell_reinitialize ()
{
  /* The default shell prompts. */
  primary_prompt = PPROMPT;
  secondary_prompt = SPROMPT;

  /* Things that get 1. */
  current_command_number = 1;

  /* We have decided that the ~/.bashrc file should not be executed
     for the invocation of each shell script.  If the variable $ENV
     (or $BASH_ENV) is set, its value is used as the name of a file
     to source. */
  no_rc = no_profile = 1;

  /* Things that get 0. */
  login_shell = make_login_shell = interactive = executing = 0;
  debugging = do_version = line_number = last_command_exit_value = 0;
  forced_interactive = interactive_shell = 0;
  subshell_environment = running_in_background = 0;
  expand_aliases = 0;
  bash_argv_initialized = 0;

  /* XXX - should we set jobs_m_flag to 0 here? */

#if defined (HISTORY)
  bash_history_reinit (enable_history_list = 0);
#endif /* HISTORY */

#if defined (RESTRICTED_SHELL)
  restricted = 0;
#endif /* RESTRICTED_SHELL */

  /* Ensure that the default startup file is used.  (Except that we don't
     execute this file for reinitialized shells). */
  bashrc_file = DEFAULT_BASHRC;

  /* Delete all variables and functions.  They will be reinitialized when
     the environment is parsed. */
  delete_all_contexts (shell_variables);
  delete_all_variables (shell_functions);

  reinit_special_variables ();

#if defined (READLINE)
  bashline_reinitialize ();
#endif

  shell_reinitialized = 1;
}