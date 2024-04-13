compose_init (compose_env_t *env)
{
  memset (env, 0, sizeof (*env));
  env->alt = multipart_alternative;
  env->attlist = attlist_copy (attachment_list);
  mu_list_foreach (add_header_list, seed_headers, env);
}