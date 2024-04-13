escape_remove_attachment (int argc, char **argv, compose_env_t *env)
{
  size_t count;
  unsigned long n;
  char *p;
  
  if (escape_check_args (argc, argv, 2, 2))
    return 1;
  n = strtoul (argv[1], &p, 10);
  if (*p)
    {
      mu_error (_("not a valid number: %s"), argv[1]);
      return 1;
    }
  
  mu_list_count (env->attlist, &count);
  if (n == 0 || n > count)
    {
      mu_error (_("index out of range"));
      return 1;
    }

  return mu_list_remove_nth (env->attlist, n - 1);
}