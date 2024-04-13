bash_vi_complete (count, key)
     int count, key;
{
#if defined (SPECIFIC_COMPLETION_FUNCTIONS)
  int p, r;
  char *t;

  if ((rl_point < rl_end) && (!whitespace (rl_line_buffer[rl_point])))
    {
      if (!whitespace (rl_line_buffer[rl_point + 1]))
	rl_vi_end_word (1, 'E');
      rl_point++;
    }

  /* Find boundaries of current word, according to vi definition of a
     `bigword'. */
  t = 0;
  if (rl_point > 0)
    {
      p = rl_point;
      rl_vi_bWord (1, 'B');
      r = rl_point;
      rl_point = p;
      p = r;

      t = substring (rl_line_buffer, p, rl_point);
    }      

  if (t && completion_glob_pattern (t) == 0)
    rl_explicit_arg = 1;	/* XXX - force glob_complete_word to append `*' */
  FREE (t);

  if (key == '*')	/* Expansion and replacement. */
    r = bash_glob_expand_word (count, key);
  else if (key == '=')	/* List possible completions. */
    r = bash_glob_list_expansions (count, key);
  else if (key == '\\')	/* Standard completion */
    r = bash_glob_complete_word (count, key);
  else
    r = rl_complete (0, key);

  if (key == '*' || key == '\\')
    rl_vi_start_inserting (key, 1, 1);

  return (r);
#else
  return rl_vi_complete (count, key);
#endif /* !SPECIFIC_COMPLETION_FUNCTIONS */
}