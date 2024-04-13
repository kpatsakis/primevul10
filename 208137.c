main_recompress_output (main_file *ofile)
{
  pid_t recomp_id;  /* One subproc. */
  int   pipefd[2];  /* One pipe. */
  int   output_fd = -1;
  int   ret;
  const main_extcomp *recomp = ofile->compressor;

  pipefd[0] = pipefd[1] = -1;

  if (pipe (pipefd))
    {
      XPR(NT "pipe failed: %s\n", xd3_mainerror (ret = get_errno ()));
      goto pipe_cleanup;
    }

  if ((recomp_id = fork ()) < 0)
    {
      XPR(NT "fork failed: %s\n", xd3_mainerror (ret = get_errno ()));
      goto pipe_cleanup;
    }

  /* The child runs the recompression process: */
  if (recomp_id == 0)
    {
      if (option_verbose > 2)
	{
	  XPR(NT "external recompression pid %d\n", getpid ());
	}

      /* Setup pipes: write to the output file, read from the pipe. */
      if (dup2 (XFNO (ofile), STDOUT_FILENO) < 0 ||
	  dup2 (pipefd[PIPE_READ_FD], STDIN_FILENO) < 0 ||
	  close (pipefd[PIPE_READ_FD]) ||
	  close (pipefd[PIPE_WRITE_FD]) ||
	  execlp (recomp->recomp_cmdname, recomp->recomp_cmdname,
		  recomp->recomp_options,
		  option_force2 ? "-f" : NULL,
		  NULL))
	{
	  XPR(NT "child process %s failed to execute: %s\n",
	      recomp->recomp_cmdname, xd3_mainerror (get_errno ()));
	}

      _exit (127);
    }

  XD3_ASSERT(num_subprocs < MAX_SUBPROCS);
  ext_subprocs[num_subprocs++] = recomp_id;

  /* The parent closes both pipes after duplicating the output-fd for
   * writing to the compression pipe. */
  output_fd = dup (pipefd[PIPE_WRITE_FD]);

  if (output_fd < 0 ||
      main_file_close (ofile) ||
      close (pipefd[PIPE_READ_FD]) ||
      close (pipefd[PIPE_WRITE_FD]))
    {
      XPR(NT "close failed: %s\n", xd3_mainerror (ret = get_errno ()));
      goto pipe_cleanup;
    }

#if XD3_STDIO
  /* Note: fdopen() acquires the fd, closes it when finished. */
  if ((ofile->file = fdopen (output_fd, "w")) == NULL)
    {
      XPR(NT "fdopen failed: %s\n", xd3_mainerror (ret = get_errno ()));
      goto pipe_cleanup;
    }

#elif XD3_POSIX
  ofile->file = output_fd;
#endif

  /* Now the output file will be compressed. */
  return 0;

 pipe_cleanup:
  close (output_fd);
  close (pipefd[PIPE_READ_FD]);
  close (pipefd[PIPE_WRITE_FD]);
  return ret;
}