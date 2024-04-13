report_multipart_type (compose_env_t *env)
{
  mu_printf ("multipart/%s\n", env->alt ? "alternative" : "mixed");
}