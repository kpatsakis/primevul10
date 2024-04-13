bash_directory_completion_matches (text)
     const char *text;
{
  char **m1;
  char *dfn;
  int qc;

  qc = rl_dispatching ? rl_completion_quote_character : 0;  
  /* If rl_completion_found_quote != 0, rl_completion_matches will call the
     filename dequoting function, causing the directory name to be dequoted
     twice. */
  if (rl_dispatching && rl_completion_found_quote == 0)
    dfn = bash_dequote_filename ((char *)text, qc);
  else
    dfn = (char *)text;
  m1 = rl_completion_matches (dfn, rl_filename_completion_function);
  if (dfn != text)
    free (dfn);

  if (m1 == 0 || m1[0] == 0)
    return m1;
  /* We don't bother recomputing the lcd of the matches, because it will just
     get thrown away by the programmable completion code and recomputed
     later. */
  (void)bash_progcomp_ignore_filenames (m1);
  return m1;
}