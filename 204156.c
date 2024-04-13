read_cdtext_generic(void *p_env)
{
  generic_img_private_t *p_user_data = p_env;
  return mmc_read_cdtext ( p_user_data->cdio );
}