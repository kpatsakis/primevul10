bash_default_completion (text, start, end, qc, compflags)
     const char *text;
     int start, end, qc, compflags;
{
  char **matches, *t;

  matches = (char **)NULL;

  /* New posix-style command substitution or variable name? */
  if (*text == '$')
    {
      if (qc != '\'' && text[1] == '(') /* ) */
	matches = rl_completion_matches (text, command_subst_completion_function);
      else
	{
	  matches = rl_completion_matches (text, variable_completion_function);
	  /* If a single match, see if it expands to a directory name and append
	     a slash if it does.  This requires us to expand the variable name,
	     so we don't want to display errors if the variable is unset.  This
	     can happen with dynamic variables whose value has never been
	     requested. */
	  if (matches && matches[0] && matches[1] == 0)
	    {
	      t = savestring (matches[0]);
	      bash_filename_stat_hook (&t);
	      /* doesn't use test_for_directory because that performs tilde
		 expansion */
	      if (file_isdir (t))
		rl_completion_append_character = '/';
	      free (t);
	    }
	}
    }

  /* If the word starts in `~', and there is no slash in the word, then
     try completing this word as a username. */
  if (matches == 0 && *text == '~' && mbschr (text, '/') == 0)
    matches = rl_completion_matches (text, rl_username_completion_function);

  /* Another one.  Why not?  If the word starts in '@', then look through
     the world of known hostnames for completion first. */
  if (matches == 0 && perform_hostname_completion && *text == '@')
    matches = rl_completion_matches (text, hostname_completion_function);

  /* And last, (but not least) if this word is in a command position, then
     complete over possible command names, including aliases, functions,
     and command names. */
  if (matches == 0 && (compflags & DEFCOMP_CMDPOS))
    {
      /* If END == START and text[0] == 0, we are trying to complete an empty
	 command word. */
      if (no_empty_command_completion && end == start && text[0] == '\0')
	{
	  matches = (char **)NULL;
	  rl_ignore_some_completions_function = bash_ignore_everything;
	}
      else
	{
#define CMD_IS_DIR(x)	(absolute_pathname(x) == 0 && absolute_program(x) == 0 && *(x) != '~' && test_for_directory (x))

	  dot_in_path = 0;
	  matches = rl_completion_matches (text, command_word_completion_function);

	  /* If we are attempting command completion and nothing matches, we
	     do not want readline to perform filename completion for us.  We
	     still want to be able to complete partial pathnames, so set the
	     completion ignore function to something which will remove
	     filenames and leave directories in the match list. */
	  if (matches == (char **)NULL)
	    rl_ignore_some_completions_function = bash_ignore_filenames;
	  else if (matches[1] == 0 && CMD_IS_DIR(matches[0]) && dot_in_path == 0)
	    /* If we found a single match, without looking in the current
	       directory (because it's not in $PATH), but the found name is
	       also a command in the current directory, suppress appending any
	       terminating character, since it's ambiguous. */
	    {
	      rl_completion_suppress_append = 1;
	      rl_filename_completion_desired = 0;
	    }
	  else if (matches[0] && matches[1] && STREQ (matches[0], matches[1]) && CMD_IS_DIR (matches[0]))
	    /* There are multiple instances of the same match (duplicate
	       completions haven't yet been removed).  In this case, all of
	       the matches will be the same, and the duplicate removal code
	       will distill them all down to one.  We turn on
	       rl_completion_suppress_append for the same reason as above.
	       Remember: we only care if there's eventually a single unique
	       completion.  If there are multiple completions this won't
	       make a difference and the problem won't occur. */
	    {
	      rl_completion_suppress_append = 1;
	      rl_filename_completion_desired = 0;
	    }
	}
    }

  /* This could be a globbing pattern, so try to expand it using pathname
     expansion. */
  if (!matches && completion_glob_pattern ((char *)text))
    {
      matches = rl_completion_matches (text, glob_complete_word);
      /* A glob expression that matches more than one filename is problematic.
	 If we match more than one filename, punt. */
      if (matches && matches[1] && rl_completion_type == TAB)
	{
	  strvec_dispose (matches);
	  matches = (char **)0;
	}
      else if (matches && matches[1] && rl_completion_type == '!')
	{
	  rl_completion_suppress_append = 1;
	  rl_filename_completion_desired = 0;
	}
    }

  return (matches);
}