regfree (preg)
    regex_t *preg;
{
  re_dfa_t *dfa = preg->buffer;
  if (BE (dfa != NULL, 1))
    {
      lock_fini (dfa->lock);
      free_dfa_content (dfa);
    }
  preg->buffer = NULL;
  preg->allocated = 0;

  re_free (preg->fastmap);
  preg->fastmap = NULL;

  re_free (preg->translate);
  preg->translate = NULL;
}