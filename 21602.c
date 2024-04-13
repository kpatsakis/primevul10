run_wordexp (words)
     char *words;
{
  int code, nw, nb;
  WORD_LIST *wl, *tl, *result;

  code = setjmp_nosigs (top_level);

  if (code != NOT_JUMPED)
    {
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
	  command_error ("run_wordexp", CMDERR_BADJUMP, code, 0);
	}
    }

  /* Run it through the parser to get a list of words and expand them */
  if (words && *words)
    {
      with_input_from_string (words, "--wordexp");
      if (parse_command () != 0)
	return (126);
      if (global_command == 0)
	{
	  printf ("0\n0\n");
	  return (0);
	}
      if (global_command->type != cm_simple)
	return (126);
      wl = global_command->value.Simple->words;
      if (protected_mode)
	for (tl = wl; tl; tl = tl->next)
	  tl->word->flags |= W_NOCOMSUB|W_NOPROCSUB;
      result = wl ? expand_words_no_vars (wl) : (WORD_LIST *)0;
    }
  else
    result = (WORD_LIST *)0;

  last_command_exit_value = 0;

  if (result == 0)
    {
      printf ("0\n0\n");
      return (0);
    }

  /* Count up the number of words and bytes, and print them.  Don't count
     the trailing NUL byte. */
  for (nw = nb = 0, wl = result; wl; wl = wl->next)
    {
      nw++;
      nb += strlen (wl->word->word);
    }
  printf ("%u\n%u\n", nw, nb);
  /* Print each word on a separate line.  This will have to be changed when
     the interface to glibc is completed. */
  for (wl = result; wl; wl = wl->next)
    printf ("%s\n", wl->word->word);

  return (0);
}