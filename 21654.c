run_shopt_alist ()
{
  register int i;

  for (i = 0; i < shopt_ind; i++)
    if (shopt_setopt (shopt_alist[i].word, (shopt_alist[i].token == '-')) != EXECUTION_SUCCESS)
      exit (EX_BADUSAGE);
  free (shopt_alist);
  shopt_alist = 0;
  shopt_ind = shopt_len = 0;
}