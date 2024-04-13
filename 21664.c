run_one_command (command)
     char *command;
{
  int code;

  code = setjmp_nosigs (top_level);

  if (code != NOT_JUMPED)
    {
#if defined (PROCESS_SUBSTITUTION)
      unlink_fifo_list ();
#endif /* PROCESS_SUBSTITUTION */
      switch (code)
	{
	  /* Some kind of throw to top_level has occurred. */
	case FORCE_EOF:
	  return last_command_exit_value = 127;
	case ERREXIT:
	case EXITPROG:
	  return last_command_exit_value;
	case DISCARD:
	  return last_command_exit_value = 1;
	default:
	  command_error ("run_one_command", CMDERR_BADJUMP, code, 0);
	}
    }
   return (parse_and_execute (savestring (command), "-c", SEVAL_NOHIST));
}