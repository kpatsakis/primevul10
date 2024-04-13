parse_shell_options (argv, arg_start, arg_end)
     char **argv;
     int arg_start, arg_end;
{
  int arg_index;
  int arg_character, on_or_off, next_arg, i;
  char *o_option, *arg_string;

  arg_index = arg_start;
  while (arg_index != arg_end && (arg_string = argv[arg_index]) &&
	 (*arg_string == '-' || *arg_string == '+'))
    {
      /* There are flag arguments, so parse them. */
      next_arg = arg_index + 1;

      /* A single `-' signals the end of options.  From the 4.3 BSD sh.
	 An option `--' means the same thing; this is the standard
	 getopt(3) meaning. */
      if (arg_string[0] == '-' &&
	   (arg_string[1] == '\0' ||
	     (arg_string[1] == '-' && arg_string[2] == '\0')))
	return (next_arg);

      i = 1;
      on_or_off = arg_string[0];
      while (arg_character = arg_string[i++])
	{
	  switch (arg_character)
	    {
	    case 'c':
	      want_pending_command = 1;
	      break;

	    case 'l':
	      make_login_shell = 1;
	      break;

	    case 's':
	      read_from_stdin = 1;
	      break;

	    case 'o':
	      o_option = argv[next_arg];
	      if (o_option == 0)
		{
		  list_minus_o_opts (-1, (on_or_off == '-') ? 0 : 1);
		  break;
		}
	      if (set_minus_o_option (on_or_off, o_option) != EXECUTION_SUCCESS)
		exit (EX_BADUSAGE);
	      next_arg++;
	      break;

	    case 'O':
	      /* Since some of these can be overridden by the normal
		 interactive/non-interactive shell initialization or
		 initializing posix mode, we save the options and process
		 them after initialization. */
	      o_option = argv[next_arg];
	      if (o_option == 0)
		{
		  shopt_listopt (o_option, (on_or_off == '-') ? 0 : 1);
		  break;
		}
	      add_shopt_to_alist (o_option, on_or_off);
	      next_arg++;
	      break;

	    case 'D':
	      dump_translatable_strings = 1;
	      break;

	    default:
	      if (change_flag (arg_character, on_or_off) == FLAG_ERROR)
		{
		  report_error (_("%c%c: invalid option"), on_or_off, arg_character);
		  show_shell_usage (stderr, 0);
		  exit (EX_BADUSAGE);
		}
	    }
	}
      /* Can't do just a simple increment anymore -- what about
	 "bash -abouo emacs ignoreeof -hP"? */
      arg_index = next_arg;
    }

  return (arg_index);
}