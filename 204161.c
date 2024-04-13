get_first_track_num_generic(void *p_user_data)
{
  const generic_img_private_t *p_env = p_user_data;

  if (!p_env->toc_init)
    p_env->cdio->op.read_toc (p_user_data);

  return p_env->toc_init ? p_env->i_first_track : CDIO_INVALID_TRACK;
}