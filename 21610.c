run_startup_files ()
{
#if defined (JOB_CONTROL)
  int old_job_control;
#endif
  int sourced_login, run_by_ssh;

  /* get the rshd/sshd case out of the way first. */
  if (interactive_shell == 0 && no_rc == 0 && login_shell == 0 &&
      act_like_sh == 0 && command_execution_string)
    {
#ifdef SSH_SOURCE_BASHRC
      run_by_ssh = (find_variable ("SSH_CLIENT") != (SHELL_VAR *)0) ||
		   (find_variable ("SSH2_CLIENT") != (SHELL_VAR *)0);
#else
      run_by_ssh = 0;
#endif

      /* If we were run by sshd or we think we were run by rshd, execute
	 ~/.bashrc if we are a top-level shell. */
      if ((run_by_ssh || isnetconn (fileno (stdin))) && shell_level < 2)
	{
#ifdef SYS_BASHRC
#  if defined (__OPENNT)
	  maybe_execute_file (_prefixInstallPath(SYS_BASHRC, NULL, 0), 1);
#  else
	  maybe_execute_file (SYS_BASHRC, 1);
#  endif
#endif
	  maybe_execute_file (bashrc_file, 1);
	  return;
	}
    }

#if defined (JOB_CONTROL)
  /* Startup files should be run without job control enabled. */
  old_job_control = interactive_shell ? set_job_control (0) : 0;
#endif

  sourced_login = 0;

  /* A shell begun with the --login (or -l) flag that is not in posix mode
     runs the login shell startup files, no matter whether or not it is
     interactive.  If NON_INTERACTIVE_LOGIN_SHELLS is defined, run the
     startup files if argv[0][0] == '-' as well. */
#if defined (NON_INTERACTIVE_LOGIN_SHELLS)
  if (login_shell && posixly_correct == 0)
#else
  if (login_shell < 0 && posixly_correct == 0)
#endif
    {
      /* We don't execute .bashrc for login shells. */
      no_rc++;

      /* Execute /etc/profile and one of the personal login shell
	 initialization files. */
      if (no_profile == 0)
	{
	  maybe_execute_file (SYS_PROFILE, 1);

	  if (act_like_sh)	/* sh */
	    maybe_execute_file ("~/.profile", 1);
	  else if ((maybe_execute_file ("~/.bash_profile", 1) == 0) &&
		   (maybe_execute_file ("~/.bash_login", 1) == 0))	/* bash */
	    maybe_execute_file ("~/.profile", 1);
	}

      sourced_login = 1;
    }

  /* A non-interactive shell not named `sh' and not in posix mode reads and
     executes commands from $BASH_ENV.  If `su' starts a shell with `-c cmd'
     and `-su' as the name of the shell, we want to read the startup files.
     No other non-interactive shells read any startup files. */
  if (interactive_shell == 0 && !(su_shell && login_shell))
    {
      if (posixly_correct == 0 && act_like_sh == 0 && privileged_mode == 0 &&
	    sourced_env++ == 0)
	execute_env_file (get_string_value ("BASH_ENV"));
      return;
    }

  /* Interactive shell or `-su' shell. */
  if (posixly_correct == 0)		  /* bash, sh */
    {
      if (login_shell && sourced_login++ == 0)
	{
	  /* We don't execute .bashrc for login shells. */
	  no_rc++;

	  /* Execute /etc/profile and one of the personal login shell
	     initialization files. */
	  if (no_profile == 0)
	    {
	      maybe_execute_file (SYS_PROFILE, 1);

	      if (act_like_sh)	/* sh */
		maybe_execute_file ("~/.profile", 1);
	      else if ((maybe_execute_file ("~/.bash_profile", 1) == 0) &&
		       (maybe_execute_file ("~/.bash_login", 1) == 0))	/* bash */
		maybe_execute_file ("~/.profile", 1);
	    }
	}

      /* bash */
      if (act_like_sh == 0 && no_rc == 0)
	{
#ifdef SYS_BASHRC
#  if defined (__OPENNT)
	  maybe_execute_file (_prefixInstallPath(SYS_BASHRC, NULL, 0), 1);
#  else
	  maybe_execute_file (SYS_BASHRC, 1);
#  endif
#endif
	  maybe_execute_file (bashrc_file, 1);
	}
      /* sh */
      else if (act_like_sh && privileged_mode == 0 && sourced_env++ == 0)
	execute_env_file (get_string_value ("ENV"));
    }
  else		/* bash --posix, sh --posix */
    {
      /* bash and sh */
      if (interactive_shell && privileged_mode == 0 && sourced_env++ == 0)
	execute_env_file (get_string_value ("ENV"));
    }

#if defined (JOB_CONTROL)
  set_job_control (old_job_control);
#endif
}