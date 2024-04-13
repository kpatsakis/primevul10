shell_expand_line (count, ignore)
     int count, ignore;
{
  char *new_line;
  WORD_LIST *expanded_string;
  WORD_DESC *w;

  new_line = 0;
#if defined (BANG_HISTORY)
  new_line = history_expand_line_internal (rl_line_buffer);
#endif

#if defined (ALIAS)
  if (new_line)
    {
      char *alias_line;

      alias_line = alias_expand (new_line);
      free (new_line);
      new_line = alias_line;
    }
#endif /* ALIAS */

  if (new_line)
    {
      int old_point = rl_point;
      int at_end = rl_point == rl_end;

      /* If the line was history and alias expanded, then make that
	 be one thing to undo. */
      maybe_make_readline_line (new_line);
      free (new_line);

      /* If there is variable expansion to perform, do that as a separate
	 operation to be undone. */

#if 1
      w = alloc_word_desc ();
      w->word = savestring (rl_line_buffer);
      w->flags = rl_explicit_arg ? (W_NOPROCSUB|W_NOCOMSUB) : 0;
      expanded_string = expand_word (w, rl_explicit_arg ? Q_HERE_DOCUMENT : 0);
      dispose_word (w);
#else
      new_line = savestring (rl_line_buffer);
      expanded_string = expand_string (new_line, 0);
      FREE (new_line);
#endif

      if (expanded_string == 0)
	{
	  new_line = (char *)xmalloc (1);
	  new_line[0] = '\0';
	}
      else
	{
	  new_line = string_list (expanded_string);
	  dispose_words (expanded_string);
	}

      maybe_make_readline_line (new_line);
      free (new_line);

      /* Place rl_point where we think it should go. */
      if (at_end)
	rl_point = rl_end;
      else if (old_point < rl_end)
	{
	  rl_point = old_point;
	  if (!whitespace (rl_line_buffer[rl_point]))
	    rl_forward_word (1, 0);
	}
      return 0;
    }
  else
    {
      cleanup_expansion_error ();
      return 1;
    }
}