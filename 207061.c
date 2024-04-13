compose_header_get (compose_env_t *env, char *name, char *defval)
{
  char const *p;

  if (mu_header_sget_value (env->header, name, &p))
    p = defval;
  return p;
}