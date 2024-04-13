history_completion_generator (hint_text, state)
     const char *hint_text;
     int state;
{
  static int local_index, len;
  static const char *text;

  /* If this is the first call to the generator, then initialize the
     list of strings to complete over. */
  if (state == 0)
    {
      if (dabbrev_expand_active)	/* This is kind of messy */
	rl_completion_suppress_append = 1;
      local_index = 0;
      build_history_completion_array ();
      text = hint_text;
      len = strlen (text);
    }

  while (history_completion_array && history_completion_array[local_index])
    {
      /* XXX - should this use completion-ignore-case? */
      if (strncmp (text, history_completion_array[local_index++], len) == 0)
	return (savestring (history_completion_array[local_index - 1]));
    }
  return ((char *)NULL);
}