seed_headers (void *item, void *data)
{
  struct add_header *hp = item;
  compose_env_t *env = data;

  compose_header_set (env, hp->name, hp->value, hp->mode);
  return 0;
}