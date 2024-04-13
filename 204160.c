cdio_generic_init (void *user_data, int open_flags)
{
  generic_img_private_t *p_env = user_data;
  if (p_env->init) {
    cdio_warn ("init called more than once");
    return false;
  }

  p_env->fd = open (p_env->source_name, open_flags, 0);

  if (p_env->fd < 0)
    {
      cdio_warn ("open (%s): %s", p_env->source_name, strerror (errno));
      return false;
    }

  p_env->init = true;
  p_env->toc_init = false;
  p_env->cdtext  = NULL;
  p_env->scsi_tuple  = NULL;
  p_env->b_cdtext_error = false;
  p_env->u_joliet_level = 0;  /* Assume no Joliet extensions initally */
  return true;
}