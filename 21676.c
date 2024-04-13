bash_execute_unix_command (count, key)
     int count;	/* ignored */
     int key;
{
  int type;
  register int i, r;
  intmax_t mi;
  sh_parser_state_t ps;
  char *cmd, *value, *ce, old_ch;
  SHELL_VAR *v;
  char ibuf[INT_STRLEN_BOUND(int) + 1];
  Keymap cmd_xmap;

  /* First, we need to find the right command to execute.  This is tricky,
     because we might have already indirected into another keymap, so we
     have to walk cmd_xmap using the entire key sequence. */
  cmd_xmap = get_cmd_xmap_from_keymap (rl_get_keymap ());
  cmd = (char *)rl_function_of_keyseq_len (rl_executing_keyseq, rl_key_sequence_length, cmd_xmap, &type);
    
  if (cmd == 0 || type != ISMACR)
    {
      rl_crlf ();
      internal_error (_("bash_execute_unix_command: cannot find keymap for command"));
      rl_forced_update_display ();
      return 1;
    }

  ce = rl_get_termcap ("ce");
  if (ce)	/* clear current line */
    {
#if 0
      fprintf (rl_outstream, "\r");
      tputs (ce, 1, putx);
#else
      rl_clear_visible_line ();
#endif
      fflush (rl_outstream);
    }
  else
    rl_crlf ();	/* move to a new line */

  v = bind_variable ("READLINE_LINE", rl_line_buffer, 0);
  if (v)
    VSETATTR (v, att_exported);
  i = rl_point;
#if defined (HANDLE_MULTIBYTE)
  if (MB_CUR_MAX > 1)
    {
      old_ch = rl_line_buffer[rl_point];
      rl_line_buffer[rl_point] = '\0';
      i = MB_STRLEN (rl_line_buffer);
      rl_line_buffer[rl_point] = old_ch;
    }
#endif
  value = inttostr (i, ibuf, sizeof (ibuf));
  v = bind_int_variable ("READLINE_POINT", value, 0);
  if (v)
    VSETATTR (v, att_exported);
  array_needs_making = 1;

  save_parser_state (&ps);
  r = parse_and_execute (savestring (cmd), "bash_execute_unix_command", SEVAL_NOHIST|SEVAL_NOFREE);
  restore_parser_state (&ps);

  v = find_variable ("READLINE_LINE");
  maybe_make_readline_line (v ? value_cell (v) : 0);

  v = find_variable ("READLINE_POINT");
  if (v && legal_number (value_cell (v), &mi))
    {
      i = mi;
#if defined (HANDLE_MULTIBYTE)
      if (i > 0 && MB_CUR_MAX > 1)
	i = _rl_find_next_mbchar (rl_line_buffer, 0, i, 0);
#endif
      if (i != rl_point)
	{
	  rl_point = i;
	  if (rl_point > rl_end)
	    rl_point = rl_end;
	  else if (rl_point < 0)
	    rl_point = 0;
	}
    }      

  check_unbind_variable ("READLINE_LINE");
  check_unbind_variable ("READLINE_POINT");
  array_needs_making = 1;

  /* and restore the readline buffer and display after command execution. */
  /* If we clear the last line of the prompt above, redraw only that last
     line.  If the command returns 124, we redraw unconditionally as in
     previous versions. */
  if (ce && r != 124)
    rl_redraw_prompt_last_line ();
  else
    rl_forced_update_display ();

  return 0;
}