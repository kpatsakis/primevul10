cdio_generic_read (void *user_data, void *buf, size_t size)
{
  generic_img_private_t *p_env = user_data;
  return read(p_env->fd, buf, size);
}