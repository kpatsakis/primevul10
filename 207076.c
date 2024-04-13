compose_destroy (compose_env_t *env)
{
  mu_header_destroy (&env->header);
  free (env->outfiles);
  mu_mime_destroy (&env->mime);
  mu_list_destroy (&env->attlist);
  mu_stream_destroy (&env->compstr);
}