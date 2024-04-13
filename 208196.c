xd3_posix_io (int fd, uint8_t *buf, size_t size,
	      xd3_posix_func *func, size_t *nread)
{
  int ret;
  size_t nproc = 0;

  while (nproc < size)
    {
      size_t tryread = min(size - nproc, 1U << 30);
      ssize_t result = (*func) (fd, buf + nproc, tryread);

      if (result < 0)
	{
	  ret = get_errno ();
	  if (ret != EAGAIN && ret != EINTR)
	    {
	      return ret;
	    }
	  continue;
	}

      if (nread != NULL && result == 0) { break; }

      nproc += result;
    }
  if (nread != NULL) { (*nread) = nproc; }
  return 0;
}