xd3_win32_io (HANDLE file, uint8_t *buf, size_t size,
	      int is_read, size_t *nread)
{
  int ret = 0;
  size_t nproc = 0;

  while (nproc < size)
    {
      DWORD nproc2 = 0;  /* hmm */
	  DWORD nremain = size - nproc;
      if ((is_read ?
	   ReadFile (file, buf + nproc, nremain, &nproc2, NULL) :
	   WriteFile (file, buf + nproc, nremain, &nproc2, NULL)) == 0)
	{
	  ret = get_errno();
	  if (ret != ERROR_HANDLE_EOF && ret != ERROR_BROKEN_PIPE)
	    {
	      return ret;
	    }
	  /* By falling through here, we'll break this loop in the
	   * read case in case of eof or broken pipe. */
	}

      nproc += nproc2;

      if (nread != NULL && nproc2 == 0) { break; }
    }
  if (nread != NULL) { (*nread) = nproc; }
  return 0;
}