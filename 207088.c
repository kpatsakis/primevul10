escape_toggle_multipart_type (int argc, char **argv, compose_env_t *env)
{
  env->alt = !env->alt;
  report_multipart_type (env);
  return 0;
}