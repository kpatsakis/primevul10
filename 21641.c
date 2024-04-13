main (argc, argv, env)
     int argc;
     char **argv, **env;
#endif /* !NO_MAIN_ENV_ARG */
{
  register int i;
  int code, old_errexit_flag;
#if defined (RESTRICTED_SHELL)
  int saverst;
#endif
  volatile int locally_skip_execution;
  volatile int arg_index, top_level_arg_index;
#ifdef __OPENNT
  char **env;

  env = environ;
#endif /* __OPENNT */

  USE_VAR(argc);
  USE_VAR(argv);
  USE_VAR(env);
  USE_VAR(code);
  USE_VAR(old_errexit_flag);
#if defined (RESTRICTED_SHELL)
  USE_VAR(saverst);
#endif

  /* Catch early SIGINTs. */
  code = setjmp_nosigs (top_level);
  if (code)
    exit (2);

  xtrace_init ();

#if defined (USING_BASH_MALLOC) && defined (DEBUG) && !defined (DISABLE_MALLOC_WRAPPERS)
  malloc_set_register (1);	/* XXX - change to 1 for malloc debugging */
#endif

  check_dev_tty ();

#ifdef __CYGWIN__
  _cygwin32_check_tmp ();
#endif /* __CYGWIN__ */

  /* Wait forever if we are debugging a login shell. */
  while (debugging_login_shell) sleep (3);

  set_default_locale ();

  running_setuid = uidget ();

  if (getenv ("POSIXLY_CORRECT") || getenv ("POSIX_PEDANTIC"))
    posixly_correct = 1;

#if defined (USE_GNU_MALLOC_LIBRARY)
  mcheck (programming_error, (void (*) ())0);
#endif /* USE_GNU_MALLOC_LIBRARY */

  if (setjmp_sigs (subshell_top_level))
    {
      argc = subshell_argc;
      argv = subshell_argv;
      env = subshell_envp;
      sourced_env = 0;
    }

  shell_reinitialized = 0;

  /* Initialize `local' variables for all `invocations' of main (). */
  arg_index = 1;
  if (arg_index > argc)
    arg_index = argc;
  command_execution_string = shell_script_filename = (char *)NULL;
  want_pending_command = locally_skip_execution = read_from_stdin = 0;
  default_input = stdin;
#if defined (BUFFERED_INPUT)
  default_buffered_input = -1;
#endif

  /* Fix for the `infinite process creation' bug when running shell scripts
     from startup files on System V. */
  login_shell = make_login_shell = 0;

  /* If this shell has already been run, then reinitialize it to a
     vanilla state. */
  if (shell_initialized || shell_name)
    {
      /* Make sure that we do not infinitely recurse as a login shell. */
      if (*shell_name == '-')
	shell_name++;

      shell_reinitialize ();
      if (setjmp_nosigs (top_level))
	exit (2);
    }

  shell_environment = env;
  set_shell_name (argv[0]);
  shell_start_time = NOW;	/* NOW now defined in general.h */

  /* Parse argument flags from the input line. */

  /* Find full word arguments first. */
  arg_index = parse_long_options (argv, arg_index, argc);
  
  if (want_initial_help)
    {
      show_shell_usage (stdout, 1);
      exit (EXECUTION_SUCCESS);
    }

  if (do_version)
    {
      show_shell_version (1);
      exit (EXECUTION_SUCCESS);
    }

  echo_input_at_read = verbose_flag;	/* --verbose given */

  /* All done with full word options; do standard shell option parsing.*/
  this_command_name = shell_name;	/* for error reporting */
  arg_index = parse_shell_options (argv, arg_index, argc);

  /* If user supplied the "--login" (or -l) flag, then set and invert
     LOGIN_SHELL. */
  if (make_login_shell)
    {
      login_shell++;
      login_shell = -login_shell;
    }

  set_login_shell ("login_shell", login_shell != 0);

  if (dump_po_strings)
    dump_translatable_strings = 1;

  if (dump_translatable_strings)
    read_but_dont_execute = 1;

  if (running_setuid && privileged_mode == 0)
    disable_priv_mode ();

  /* Need to get the argument to a -c option processed in the
     above loop.  The next arg is a command to execute, and the
     following args are $0...$n respectively. */
  if (want_pending_command)
    {
      command_execution_string = argv[arg_index];
      if (command_execution_string == 0)
	{
	  report_error (_("%s: option requires an argument"), "-c");
	  exit (EX_BADUSAGE);
	}
      arg_index++;
    }
  this_command_name = (char *)NULL;

  /* First, let the outside world know about our interactive status.
     A shell is interactive if the `-i' flag was given, or if all of
     the following conditions are met:
	no -c command
	no arguments remaining or the -s flag given
	standard input is a terminal
	standard error is a terminal
     Refer to Posix.2, the description of the `sh' utility. */

  if (forced_interactive ||		/* -i flag */
      (!command_execution_string &&	/* No -c command and ... */
       wordexp_only == 0 &&		/* No --wordexp and ... */
       ((arg_index == argc) ||		/*   no remaining args or... */
	read_from_stdin) &&		/*   -s flag with args, and */
       isatty (fileno (stdin)) &&	/* Input is a terminal and */
       isatty (fileno (stderr))))	/* error output is a terminal. */
    init_interactive ();
  else
    init_noninteractive ();

  /*
   * Some systems have the bad habit of starting login shells with lots of open
   * file descriptors.  For instance, most systems that have picked up the
   * pre-4.0 Sun YP code leave a file descriptor open each time you call one
   * of the getpw* functions, and it's set to be open across execs.  That
   * means one for login, one for xterm, one for shelltool, etc.  There are
   * also systems that open persistent FDs to other agents or files as part
   * of process startup; these need to be set to be close-on-exec.
   */
  if (login_shell && interactive_shell)
    {
      for (i = 3; i < 20; i++)
	SET_CLOSE_ON_EXEC (i);
    }

  /* If we're in a strict Posix.2 mode, turn on interactive comments,
     alias expansion in non-interactive shells, and other Posix.2 things. */
  if (posixly_correct)
    {
      bind_variable ("POSIXLY_CORRECT", "y", 0);
      sv_strict_posix ("POSIXLY_CORRECT");
    }

  /* Now we run the shopt_alist and process the options. */
  if (shopt_alist)
    run_shopt_alist ();

  /* From here on in, the shell must be a normal functioning shell.
     Variables from the environment are expected to be set, etc. */
  shell_initialize ();

  set_default_lang ();
  set_default_locale_vars ();

  /*
   * M-x term -> TERM=eterm-color INSIDE_EMACS='251,term:0.96' (eterm)
   * M-x shell -> TERM='dumb' INSIDE_EMACS='25.1,comint' (no line editing)
   *
   * Older versions of Emacs may set EMACS to 't' or to something like
   * '22.1 (term:0.96)' instead of (or in addition to) setting INSIDE_EMACS.
   * They may set TERM to 'eterm' instead of 'eterm-color'.  They may have
   * a now-obsolete command that sets neither EMACS nor INSIDE_EMACS:
   * M-x terminal -> TERM='emacs-em7955' (line editing)
   */
  if (interactive_shell)
    {
      char *term, *emacs, *inside_emacs;
      int emacs_term, in_emacs;

      term = get_string_value ("TERM");
      emacs = get_string_value ("EMACS");
      inside_emacs = get_string_value ("INSIDE_EMACS");

      if (inside_emacs)
	{
	  emacs_term = strstr (inside_emacs, ",term:") != 0;
	  in_emacs = 1;
	}
      else if (emacs)
	{
	  /* Infer whether we are in an older Emacs. */
	  emacs_term = strstr (emacs, " (term:") != 0;
	  in_emacs = emacs_term || STREQ (emacs, "t");
	}
      else
	in_emacs = emacs_term = 0;

      /* Not sure any emacs terminal emulator sets TERM=emacs any more */
      no_line_editing |= STREQ (term, "emacs");
      no_line_editing |= in_emacs && STREQ (term, "dumb");

      /* running_under_emacs == 2 for `eterm' */
      running_under_emacs = in_emacs || STREQN (term, "emacs", 5);
      running_under_emacs += emacs_term && STREQN (term, "eterm", 5);

      if (running_under_emacs)
	gnu_error_format = 1;
    }

  top_level_arg_index = arg_index;
  old_errexit_flag = exit_immediately_on_error;

  /* Give this shell a place to longjmp to before executing the
     startup files.  This allows users to press C-c to abort the
     lengthy startup. */
  code = setjmp_sigs (top_level);
  if (code)
    {
      if (code == EXITPROG || code == ERREXIT)
	exit_shell (last_command_exit_value);
      else
	{
#if defined (JOB_CONTROL)
	  /* Reset job control, since run_startup_files turned it off. */
	  set_job_control (interactive_shell);
#endif
	  /* Reset value of `set -e', since it's turned off before running
	     the startup files. */
	  exit_immediately_on_error += old_errexit_flag;
	  locally_skip_execution++;
	}
    }

  arg_index = top_level_arg_index;

  /* Execute the start-up scripts. */

  if (interactive_shell == 0)
    {
      unbind_variable ("PS1");
      unbind_variable ("PS2");
      interactive = 0;
#if 0
      /* This has already been done by init_noninteractive */
      expand_aliases = posixly_correct;
#endif
    }
  else
    {
      change_flag ('i', FLAG_ON);
      interactive = 1;
    }

#if defined (RESTRICTED_SHELL)
  /* Set restricted_shell based on whether the basename of $0 indicates that
     the shell should be restricted or if the `-r' option was supplied at
     startup. */
  restricted_shell = shell_is_restricted (shell_name);

  /* If the `-r' option is supplied at invocation, make sure that the shell
     is not in restricted mode when running the startup files. */
  saverst = restricted;
  restricted = 0;
#endif

  /* Set positional parameters before running startup files. top_level_arg_index
     holds the index of the current argument before setting the positional
     parameters, so any changes performed in the startup files won't affect
     later option processing. */
  if (wordexp_only)
    ;			/* nothing yet */
  else if (command_execution_string)
    arg_index = bind_args (argv, arg_index, argc, 0);	/* $0 ... $n */
  else if (arg_index != argc && read_from_stdin == 0)
    {
      shell_script_filename = argv[arg_index++];
      arg_index = bind_args (argv, arg_index, argc, 1);	/* $1 ... $n */
    }
  else
    arg_index = bind_args (argv, arg_index, argc, 1);	/* $1 ... $n */

  /* The startup files are run with `set -e' temporarily disabled. */
  if (locally_skip_execution == 0 && running_setuid == 0)
    {
      old_errexit_flag = exit_immediately_on_error;
      exit_immediately_on_error = 0;

      run_startup_files ();
      exit_immediately_on_error += old_errexit_flag;
    }

  /* If we are invoked as `sh', turn on Posix mode. */
  if (act_like_sh)
    {
      bind_variable ("POSIXLY_CORRECT", "y", 0);
      sv_strict_posix ("POSIXLY_CORRECT");
    }

#if defined (RESTRICTED_SHELL)
  /* Turn on the restrictions after executing the startup files.  This
     means that `bash -r' or `set -r' invoked from a startup file will
     turn on the restrictions after the startup files are executed. */
  restricted = saverst || restricted;
  if (shell_reinitialized == 0)
    maybe_make_restricted (shell_name);
#endif /* RESTRICTED_SHELL */

#if defined (WORDEXP_OPTION)
  if (wordexp_only)
    {
      startup_state = 3;
      last_command_exit_value = run_wordexp (argv[top_level_arg_index]);
      exit_shell (last_command_exit_value);
    }
#endif

  cmd_init ();		/* initialize the command object caches */
  uwp_init ();

  if (command_execution_string)
    {
      startup_state = 2;

      if (debugging_mode)
	start_debugger ();

#if defined (ONESHOT)
      executing = 1;
      run_one_command (command_execution_string);
      exit_shell (last_command_exit_value);
#else /* ONESHOT */
      with_input_from_string (command_execution_string, "-c");
      goto read_and_execute;
#endif /* !ONESHOT */
    }

  /* Get possible input filename and set up default_buffered_input or
     default_input as appropriate. */
  if (shell_script_filename)
    open_shell_script (shell_script_filename);
  else if (interactive == 0)
    {
      /* In this mode, bash is reading a script from stdin, which is a
	 pipe or redirected file. */
#if defined (BUFFERED_INPUT)
      default_buffered_input = fileno (stdin);	/* == 0 */
#else
      setbuf (default_input, (char *)NULL);
#endif /* !BUFFERED_INPUT */
      read_from_stdin = 1;
    }
  else if (top_level_arg_index == argc)		/* arg index before startup files */
    /* "If there are no operands and the -c option is not specified, the -s
       option shall be assumed." */
    read_from_stdin = 1;

  set_bash_input ();

  if (debugging_mode && locally_skip_execution == 0 && running_setuid == 0 && (reading_shell_script || interactive_shell == 0))
    start_debugger ();

  /* Do the things that should be done only for interactive shells. */
  if (interactive_shell)
    {
      /* Set up for checking for presence of mail. */
      reset_mail_timer ();
      init_mail_dates ();

#if defined (HISTORY)
      /* Initialize the interactive history stuff. */
      bash_initialize_history ();
      /* Don't load the history from the history file if we've already
	 saved some lines in this session (e.g., by putting `history -s xx'
	 into one of the startup files). */
      if (shell_initialized == 0 && history_lines_this_session == 0)
	load_history ();
#endif /* HISTORY */

      /* Initialize terminal state for interactive shells after the
	 .bash_profile and .bashrc are interpreted. */
      get_tty_state ();
    }

#if !defined (ONESHOT)
 read_and_execute:
#endif /* !ONESHOT */

  shell_initialized = 1;

  if (pretty_print_mode && interactive_shell)
    {
      internal_warning (_("pretty-printing mode ignored in interactive shells"));
      pretty_print_mode = 0;
    }
  if (pretty_print_mode)
    exit_shell (pretty_print_loop ());

  /* Read commands until exit condition. */
  reader_loop ();
  exit_shell (last_command_exit_value);
}