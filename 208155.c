main_pipe_copier (uint8_t     *pipe_buf,
		  usize_t      pipe_bufsize,
		  size_t       nread,
		  main_file   *ifile,
		  int          outfd)
{
  int ret;
  xoff_t skipped = 0;

  /* Prevent SIGPIPE signals, allow EPIPE return values instead.  This
   * is safe to comment-out, except that the -F flag will not work
   * properly (the parent would need to treat WTERMSIG(status) ==
   * SIGPIPE). */
  struct sigaction sa;
  sa.sa_handler = SIG_IGN;
  sigaction (SIGPIPE, &sa, NULL);

  for (;;)
    {
      /* force_drain will be set when option_force and EPIPE cause us
       * to skip data.  This is reset each time through the loop, so
       * the break condition below works. */
      int force_drain = 0;
      if (nread > 0 && (ret = main_pipe_write (outfd, pipe_buf, nread)))
	{
	  if (ret == EPIPE)
	    {
	      /* This causes the loop to continue reading until nread
	       * == 0. */
	      skipped += nread;
	      force_drain = 1;
	    }
	  else
	    {
	      XPR(NT "pipe write failed: %s\n", xd3_mainerror (ret));
	      return ret;
	    }
	}

      if (nread < pipe_bufsize && !force_drain)
	{
	  break;
	}

      if ((ret = main_file_read (ifile, pipe_buf, pipe_bufsize,
				 & nread, "pipe read failed")) < 0)
	{
	  return ret;
	}
    }

  if (option_verbose && skipped != 0)
    {
      XPR(NT "skipping %"Q"u bytes in %s\n",
	  skipped, ifile->filename);
    }
  return 0;
}