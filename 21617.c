maybe_make_restricted (name)
     char *name;
{
  char *temp;

  temp = base_pathname (name);
  if (*temp == '-')
    temp++;
  if (restricted || (STREQ (temp, RESTRICTED_SHELL_NAME)))
    {
#if defined (RBASH_STATIC_PATH_VALUE)
      bind_variable ("PATH", RBASH_STATIC_PATH_VALUE, 0);
      stupidly_hack_special_variables ("PATH");		/* clear hash table */
#endif
      set_var_read_only ("PATH");
      set_var_read_only ("SHELL");
      set_var_read_only ("ENV");
      set_var_read_only ("BASH_ENV");
      restricted = 1;
    }
  return (restricted);
}