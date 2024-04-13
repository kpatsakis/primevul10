attempt_shell_completion (text, start, end)
     const char *text;
     int start, end;
{
  int in_command_position, ti, qc, dflags;
  char **matches, *command_separator_chars;
#if defined (PROGRAMMABLE_COMPLETION)
  int have_progcomps, was_assignment;
  COMPSPEC *iw_compspec;
#endif

  command_separator_chars = COMMAND_SEPARATORS;
  matches = (char **)NULL;
  rl_ignore_some_completions_function = filename_completion_ignore;

  rl_filename_quote_characters = default_filename_quote_characters;
  set_filename_bstab (rl_filename_quote_characters);
  set_directory_hook ();
  rl_filename_stat_hook = bash_filename_stat_hook;

  rl_sort_completion_matches = 1;	/* sort by default */

  /* Determine if this could be a command word.  It is if it appears at
     the start of the line (ignoring preceding whitespace), or if it
     appears after a character that separates commands.  It cannot be a
     command word if we aren't at the top-level prompt. */
  ti = start - 1;
  qc = -1;

  while ((ti > -1) && (whitespace (rl_line_buffer[ti])))
    ti--;

#if 1
  /* If this is an open quote, maybe we're trying to complete a quoted
     command name. */
  if (ti >= 0 && (rl_line_buffer[ti] == '"' || rl_line_buffer[ti] == '\''))
    {
      qc = rl_line_buffer[ti];
      ti--;
      while (ti > -1 && (whitespace (rl_line_buffer[ti])))
	ti--;
    }
#endif
      
  in_command_position = 0;
  if (ti < 0)
    {
      /* Only do command completion at the start of a line when we
	 are prompting at the top level. */
      if (current_prompt_string == ps1_prompt)
	in_command_position++;
      else if (parser_in_command_position ())
	in_command_position++;
    }
  else if (member (rl_line_buffer[ti], command_separator_chars))
    {
      in_command_position++;

      if (check_redir (ti) == 1)
	in_command_position = 0;
    }
  else
    {
      /* This still could be in command position.  It is possible
	 that all of the previous words on the line are variable
	 assignments. */
    }

  if (in_command_position && invalid_completion (text, ti))
    {
      rl_attempted_completion_over = 1;
      return ((char **)NULL);
    }

  /* Check that we haven't incorrectly flagged a closed command substitution
     as indicating we're in a command position. */
  if (in_command_position && ti >= 0 && rl_line_buffer[ti] == '`' &&
	*text != '`' && unclosed_pair (rl_line_buffer, end, "`") == 0)
    in_command_position = 0;

  /* Special handling for command substitution.  If *TEXT is a backquote,
     it can be the start or end of an old-style command substitution, or
     unmatched.  If it's unmatched, both calls to unclosed_pair will
     succeed.  Don't bother if readline found a single quote and we are
     completing on the substring.  */
  if (*text == '`' && rl_completion_quote_character != '\'' &&
	(in_command_position || (unclosed_pair (rl_line_buffer, start, "`") &&
				 unclosed_pair (rl_line_buffer, end, "`"))))
    matches = rl_completion_matches (text, command_subst_completion_function);

#if defined (PROGRAMMABLE_COMPLETION)
  /* Attempt programmable completion. */
  have_progcomps = prog_completion_enabled && (progcomp_size () > 0);
  iw_compspec = progcomp_search (INITIALWORD);
  if (matches == 0 &&
      (in_command_position == 0 || text[0] == '\0' || (in_command_position && iw_compspec)) &&
      current_prompt_string == ps1_prompt)
    {
      int s, e, s1, e1, os, foundcs;
      char *n;

      /* XXX - don't free the members */
      if (prog_complete_matches)
	free (prog_complete_matches);
      prog_complete_matches = (char **)NULL;

      os = start;
      n = 0;
      was_assignment = 0;
      s = find_cmd_start (os);
      e = find_cmd_end (end);
      do
	{
	  /* Don't read past the end of rl_line_buffer */
	  if (s > rl_end)
	    {
	      s1 = s = e1;
	      break;
	    }
	  /* Or past point if point is within an assignment statement */
	  else if (was_assignment && s > rl_point)
	    {
	      s1 = s = e1;
	      break;
	    }
	  /* Skip over assignment statements preceding a command name.  If we
	     don't find a command name at all, we can perform command name
	     completion.  If we find a partial command name, we should perform
	     command name completion on it. */
	  FREE (n);
	  n = find_cmd_name (s, &s1, &e1);
	  s = e1 + 1;
	}
      while (was_assignment = assignment (n, 0));
      s = s1;		/* reset to index where name begins */

      /* s == index of where command name begins (reset above)
	 e == end of current command, may be end of line
         s1 = index of where command name begins
         e1 == index of where command name ends
	 start == index of where word to be completed begins
	 end == index of where word to be completed ends
	 if (s == start) we are doing command word completion for sure
	 if (e1 == end) we are at the end of the command name and completing it */
      if (start == 0 && end == 0 && e != 0 && text[0] == '\0')	/* beginning of non-empty line */
        foundcs = 0;
      else if (start == end && start == s1 && e != 0 && e1 > end)	/* beginning of command name, leading whitespace */
	foundcs = 0;
      else if (e == 0 && e == s && text[0] == '\0' && have_progcomps)	/* beginning of empty line */
        prog_complete_matches = programmable_completions (EMPTYCMD, text, s, e, &foundcs);
      else if (start == end && text[0] == '\0' && s1 > start && whitespace (rl_line_buffer[start]))
        foundcs = 0;		/* whitespace before command name */
      else if (e > s && was_assignment == 0 && e1 == end && rl_line_buffer[e] == 0 && whitespace (rl_line_buffer[e-1]) == 0)
	{
	  /* not assignment statement, but still want to perform command
	     completion if we are composing command word. */
	  foundcs = 0;
	  in_command_position = s == start && STREQ (n, text);	/* XXX */
	}
      else if (e > s && was_assignment == 0 && have_progcomps)
	{
	  prog_complete_matches = programmable_completions (n, text, s, e, &foundcs);
	  /* command completion if programmable completion fails */
	  /* If we have a completion for the initial word, we can prefer that */
	  in_command_position = s == start && (iw_compspec || STREQ (n, text));	/* XXX */
	  if (iw_compspec && in_command_position)
	    foundcs = 0;
	}
      /* empty command name following command separator */
      else if (s >= e && n[0] == '\0' && text[0] == '\0' && start > 0 &&
		was_assignment == 0 && member (rl_line_buffer[start-1], COMMAND_SEPARATORS))
	{
	  foundcs = 0;
	  in_command_position = 1;
	}
      else if (s >= e && n[0] == '\0' && text[0] == '\0' && start > 0)
        {
          foundcs = 0;	/* empty command name following optional assignments */
          in_command_position += was_assignment;
        }
      else if (s == start && e == end && STREQ (n, text) && start > 0)
        {
          foundcs = 0;	/* partial command name following assignments */
          in_command_position = 1;
        }
      else
	foundcs = 0;

      /* If we have defined a compspec for the initial (command) word, call
	 it and process the results like any other programmable completion. */
      if (in_command_position && have_progcomps && foundcs == 0 && iw_compspec)
	prog_complete_matches = programmable_completions (INITIALWORD, text, s, e, &foundcs);

      FREE (n);
      /* XXX - if we found a COMPSPEC for the command, just return whatever
	 the programmable completion code returns, and disable the default
	 filename completion that readline will do unless the COPT_DEFAULT
	 option has been set with the `-o default' option to complete or
	 compopt. */
      if (foundcs)
	{
	  pcomp_set_readline_variables (foundcs, 1);
	  /* Turn what the programmable completion code returns into what
	     readline wants.  I should have made compute_lcd_of_matches
	     external... */
	  matches = rl_completion_matches (text, prog_complete_return);
	  if ((foundcs & COPT_DEFAULT) == 0)
	    rl_attempted_completion_over = 1;	/* no default */
	  if (matches || ((foundcs & COPT_BASHDEFAULT) == 0))
	    return (matches);
	}
    }
#endif

  if (matches == 0)
    {
      dflags = 0;
      if (in_command_position)
	dflags |= DEFCOMP_CMDPOS;
      matches = bash_default_completion (text, start, end, qc, dflags);
    }

  return matches;
}