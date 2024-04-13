cdio_generic_stdio_free (void *p_user_data)
{
  generic_img_private_t *p_env = p_user_data;

  if (NULL == p_env) return;
  if (NULL != p_env->source_name)
    free (p_env->source_name);

  if (p_env->data_source)
    cdio_stdio_destroy (p_env->data_source);
}