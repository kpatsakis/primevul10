main_input_decompress_setup (const main_extcomp   *decomp,
			     main_file            *ifile,
			     uint8_t              *input_buf,
			     usize_t               input_bufsize,
			     uint8_t              *pipe_buf,
			     usize_t               pipe_bufsize,
			     usize_t               pipe_avail,
			     size_t               *nread)
{
  /* The two pipes: input and output file descriptors. */
  int outpipefd[2], inpipefd[2];
  int input_fd = -1;  /* The resulting input_fd (output of decompression). */
  pid_t decomp_id, copier_id;  /* The two subprocs. */
  int ret;

  outpipefd[0] = outpipefd[1] = -1;
  inpipefd[0]  = inpipefd[1]  = -1;

  if (pipe (outpipefd) || pipe (inpipefd))
    {
      XPR(NT "pipe failed: %s\n", xd3_mainerror (ret = get_errno ()));
      goto pipe_cleanup;
    }

  if ((decomp_id = fork ()) < 0)
    {
      XPR(NT "fork failed: %s\n", xd3_mainerror (ret = get_errno ()));
      goto pipe_cleanup;
    }

  /* The first child runs the decompression process: */
  if (decomp_id == 0)
    {
      if (option_verbose > 2)
	{
	  XPR(NT "external decompression pid %d\n", getpid ());
	}

      /* Setup pipes: write to the outpipe, read from the inpipe. */
      if (dup2 (outpipefd[PIPE_WRITE_FD], STDOUT_FILENO) < 0 ||
	  dup2 (inpipefd[PIPE_READ_FD], STDIN_FILENO) < 0 ||
	  close (outpipefd[PIPE_READ_FD]) ||
	  close (outpipefd[PIPE_WRITE_FD]) ||
	  close (inpipefd[PIPE_READ_FD]) ||
	  close (inpipefd[PIPE_WRITE_FD]) ||
	  execlp (decomp->decomp_cmdname, decomp->decomp_cmdname,
		  decomp->decomp_options,
		  option_force2 ? "-f" : NULL,
		  NULL))
	{
	  XPR(NT "child process %s failed to execute: %s\n",
	      decomp->decomp_cmdname, xd3_mainerror (get_errno ()));
	}

      _exit (127);
    }

  XD3_ASSERT(num_subprocs < MAX_SUBPROCS);
  ext_subprocs[num_subprocs++] = decomp_id;

  if ((copier_id = fork ()) < 0)
    {
      XPR(NT "fork failed: %s\n", xd3_mainerror (ret = get_errno ()));
      goto pipe_cleanup;
    }

  /* The second child runs the copier process: */
  if (copier_id == 0)
    {
      int exitval = 0;

      if (option_verbose > 2)
	{
	  XPR(NT "child pipe-copier pid %d\n", getpid ());
	}

      if (close (inpipefd[PIPE_READ_FD]) ||
	  close (outpipefd[PIPE_READ_FD]) ||
	  close (outpipefd[PIPE_WRITE_FD]) ||
	  main_pipe_copier (pipe_buf, pipe_bufsize, pipe_avail,
			    ifile, inpipefd[PIPE_WRITE_FD]) ||
	  close (inpipefd[PIPE_WRITE_FD]))
	{
	  XPR(NT "child copier process failed: %s\n",
	      xd3_mainerror (get_errno ()));
	  exitval = 1;
	}

      _exit (exitval);
    }

  XD3_ASSERT(num_subprocs < MAX_SUBPROCS);
  ext_subprocs[num_subprocs++] = copier_id;

  /* The parent closes both pipes after duplicating the output of
   * compression. */
  input_fd = dup (outpipefd[PIPE_READ_FD]);

  if (input_fd < 0 ||
      main_file_close (ifile) ||
      close (outpipefd[PIPE_READ_FD]) ||
      close (outpipefd[PIPE_WRITE_FD]) ||
      close (inpipefd[PIPE_READ_FD]) ||
      close (inpipefd[PIPE_WRITE_FD]))
    {
      XPR(NT "dup/close failed: %s\n", xd3_mainerror (ret = get_errno ()));
      goto pipe_cleanup;
    }

#if XD3_STDIO
  /* Note: fdopen() acquires the fd, closes it when finished. */
  if ((ifile->file = fdopen (input_fd, "r")) == NULL)
    {
      XPR(NT "fdopen failed: %s\n", xd3_mainerror (ret = get_errno ()));
      goto pipe_cleanup;
    }

#elif XD3_POSIX
  ifile->file = input_fd;
#endif

  ifile->compressor = decomp;

  /* Now the input file is decompressed. */
  return main_file_read (ifile, input_buf, input_bufsize,
			 nread, "input decompression failed");

 pipe_cleanup:
  close (input_fd);
  close (outpipefd[PIPE_READ_FD]);
  close (outpipefd[PIPE_WRITE_FD]);
  close (inpipefd[PIPE_READ_FD]);
  close (inpipefd[PIPE_WRITE_FD]);
  return ret;
}