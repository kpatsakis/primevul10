wgetrc_env_file_name (void)
{
  char *env = getenv ("WGETRC");
  if (env && *env)
    {
      if (!file_exists_p (env))
        {
          fprintf (stderr, _("%s: WGETRC points to %s, which doesn't exist.\n"),
                   exec_name, env);
          exit (WGET_EXIT_GENERIC_ERROR);
        }
      return xstrdup (env);
    }
  return NULL;
}