cdio_generic_free (void *p_user_data)
{
  generic_img_private_t *p_env = p_user_data;

  if (NULL == p_env) return;
  if (p_env->source_name) free (p_env->source_name);

  if (NULL != p_env->cdtext) {
      cdtext_destroy(p_env->cdtext);
      p_env->cdtext = NULL;
  }

  if (p_env->fd >= 0)
    close (p_env->fd);

  if (p_env->scsi_tuple != NULL)
    free (p_env->scsi_tuple);

  free (p_env);
}