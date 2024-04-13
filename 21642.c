parse_long_options (argv, arg_start, arg_end)
     char **argv;
     int arg_start, arg_end;
{
  int arg_index, longarg, i;
  char *arg_string;

  arg_index = arg_start;
  while ((arg_index != arg_end) && (arg_string = argv[arg_index]) &&
	 (*arg_string == '-'))
    {
      longarg = 0;

      /* Make --login equivalent to -login. */
      if (arg_string[1] == '-' && arg_string[2])
	{
	  longarg = 1;
	  arg_string++;
	}

      for (i = 0; long_args[i].name; i++)
	{
	  if (STREQ (arg_string + 1, long_args[i].name))
	    {
	      if (long_args[i].type == Int)
		*long_args[i].int_value = 1;
	      else if (argv[++arg_index] == 0)
		{
		  report_error (_("%s: option requires an argument"), long_args[i].name);
		  exit (EX_BADUSAGE);
		}
	      else
		*long_args[i].char_value = argv[arg_index];

	      break;
	    }
	}
      if (long_args[i].name == 0)
	{
	  if (longarg)
	    {
	      report_error (_("%s: invalid option"), argv[arg_index]);
	      show_shell_usage (stderr, 0);
	      exit (EX_BADUSAGE);
	    }
	  break;		/* No such argument.  Maybe flag arg. */
	}

      arg_index++;
    }

  return (arg_index);
}