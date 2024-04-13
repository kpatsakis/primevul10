main_pipe_write (int outfd, uint8_t *exist_buf, usize_t remain)
{
  int ret;

  if ((ret = xd3_posix_io (outfd, exist_buf, remain,
			   (xd3_posix_func*) &write, NULL)))
    {
      return ret;
    }

  return 0;
}