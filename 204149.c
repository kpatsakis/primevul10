get_track_channels_generic(const void *p_user_data, track_t i_track)
{
  const generic_img_private_t *p_env = p_user_data;
  return p_env->track_flags[i_track].channels;
}