edit_and_execute_command (count, c, editing_mode, edit_command)
     int count, c, editing_mode;
     char *edit_command;
{
  char *command, *metaval;
  int r, rrs, metaflag;
  sh_parser_state_t ps;

  rrs = rl_readline_state;
  saved_command_line_count = current_command_line_count;

  /* Accept the current line. */
  rl_newline (1, c);

  if (rl_explicit_arg)
    {
      command = (char *)xmalloc (strlen (edit_command) + 8);
      sprintf (command, "%s %d", edit_command, count);
    }
  else
    {
      /* Take the command we were just editing, add it to the history file,
	 then call fc to operate on it.  We have to add a dummy command to
	 the end of the history because fc ignores the last command (assumes
	 it's supposed to deal with the command before the `fc'). */
      /* This breaks down when using command-oriented history and are not
	 finished with the command, so we should not ignore the last command */
      using_history ();
      current_command_line_count++;	/* for rl_newline above */
      bash_add_history (rl_line_buffer);
      current_command_line_count = 0;	/* for dummy history entry */
      bash_add_history ("");
      history_lines_this_session++;
      using_history ();
      command = savestring (edit_command);
    }

  metaval = rl_variable_value ("input-meta");
  metaflag = RL_BOOLEAN_VARIABLE_VALUE (metaval);
  
  if (rl_deprep_term_function)
    (*rl_deprep_term_function) ();
  save_parser_state (&ps);
  r = parse_and_execute (command, (editing_mode == VI_EDITING_MODE) ? "v" : "C-xC-e", SEVAL_NOHIST);
  restore_parser_state (&ps);
  if (rl_prep_term_function)
    (*rl_prep_term_function) (metaflag);

  current_command_line_count = saved_command_line_count;

  /* Now erase the contents of the current line and undo the effects of the
     rl_accept_line() above.  We don't even want to make the text we just
     executed available for undoing. */
  rl_line_buffer[0] = '\0';	/* XXX */
  rl_point = rl_end = 0;
  rl_done = 0;
  rl_readline_state = rrs;

#if defined (VI_MODE)
  if (editing_mode == VI_EDITING_MODE)
    rl_vi_insertion_mode (1, c);
#endif

  rl_forced_update_display ();

  return r;
}