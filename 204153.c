cdio_generic_lseek (void *user_data, off_t offset, int whence)
{
  generic_img_private_t *p_env = user_data;
  return CDIO_LSEEK(p_env->fd, offset, whence);
}