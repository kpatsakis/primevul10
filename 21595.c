open_shell_script (script_name)
     char *script_name;
{
  int fd, e, fd_is_tty;
  char *filename, *path_filename, *t;
  char sample[80];
  int sample_len;
  struct stat sb;
#if defined (ARRAY_VARS)
  SHELL_VAR *funcname_v, *bash_source_v, *bash_lineno_v;
  ARRAY *funcname_a, *bash_source_a, *bash_lineno_a;
#endif

  filename = savestring (script_name);

  fd = open (filename, O_RDONLY);
  if ((fd < 0) && (errno == ENOENT) && (absolute_program (filename) == 0))
    {
      e = errno;
      /* If it's not in the current directory, try looking through PATH
	 for it. */
      path_filename = find_path_file (script_name);
      if (path_filename)
	{
	  free (filename);
	  filename = path_filename;
	  fd = open (filename, O_RDONLY);
	}
      else
	errno = e;
    }

  if (fd < 0)
    {
      e = errno;
      file_error (filename);
#if defined (JOB_CONTROL)
      end_job_control ();	/* just in case we were run as bash -i script */
#endif
      sh_exit ((e == ENOENT) ? EX_NOTFOUND : EX_NOINPUT);
    }

  free (dollar_vars[0]);
  dollar_vars[0] = exec_argv0 ? savestring (exec_argv0) : savestring (script_name);
  if (exec_argv0)
    {
      free (exec_argv0);
      exec_argv0 = (char *)NULL;
    }

  if (file_isdir (filename))
    {
#if defined (EISDIR)
      errno = EISDIR;
#else
      errno = EINVAL;
#endif
      file_error (filename);
#if defined (JOB_CONTROL)
      end_job_control ();	/* just in case we were run as bash -i script */
#endif
      sh_exit (EX_NOINPUT);
    }

#if defined (ARRAY_VARS)
  GET_ARRAY_FROM_VAR ("FUNCNAME", funcname_v, funcname_a);
  GET_ARRAY_FROM_VAR ("BASH_SOURCE", bash_source_v, bash_source_a);
  GET_ARRAY_FROM_VAR ("BASH_LINENO", bash_lineno_v, bash_lineno_a);

  array_push (bash_source_a, filename);
  if (bash_lineno_a)
    {
      t = itos (executing_line_number ());
      array_push (bash_lineno_a, t);
      free (t);
    }
  array_push (funcname_a, "main");
#endif

#ifdef HAVE_DEV_FD
  fd_is_tty = isatty (fd);
#else
  fd_is_tty = 0;
#endif

  /* Only do this with non-tty file descriptors we can seek on. */
  if (fd_is_tty == 0 && (lseek (fd, 0L, 1) != -1))
    {
      /* Check to see if the `file' in `bash file' is a binary file
	 according to the same tests done by execute_simple_command (),
	 and report an error and exit if it is. */
      sample_len = read (fd, sample, sizeof (sample));
      if (sample_len < 0)
	{
	  e = errno;
	  if ((fstat (fd, &sb) == 0) && S_ISDIR (sb.st_mode))
	    {
#if defined (EISDIR)
	      errno = EISDIR;
	      file_error (filename);
#else	      
	      internal_error (_("%s: Is a directory"), filename);
#endif
	    }
	  else
	    {
	      errno = e;
	      file_error (filename);
	    }
#if defined (JOB_CONTROL)
	  end_job_control ();	/* just in case we were run as bash -i script */
#endif
	  exit (EX_NOEXEC);
	}
      else if (sample_len > 0 && (check_binary_file (sample, sample_len)))
	{
	  internal_error (_("%s: cannot execute binary file"), filename);
#if defined (JOB_CONTROL)
	  end_job_control ();	/* just in case we were run as bash -i script */
#endif
	  exit (EX_BINARY_FILE);
	}
      /* Now rewind the file back to the beginning. */
      lseek (fd, 0L, 0);
    }

  /* Open the script.  But try to move the file descriptor to a randomly
     large one, in the hopes that any descriptors used by the script will
     not match with ours. */
  fd = move_to_high_fd (fd, 1, -1);

#if defined (BUFFERED_INPUT)
  default_buffered_input = fd;
  SET_CLOSE_ON_EXEC (default_buffered_input);
#else /* !BUFFERED_INPUT */
  default_input = fdopen (fd, "r");

  if (default_input == 0)
    {
      file_error (filename);
      exit (EX_NOTFOUND);
    }

  SET_CLOSE_ON_EXEC (fd);
  if (fileno (default_input) != fd)
    SET_CLOSE_ON_EXEC (fileno (default_input));
#endif /* !BUFFERED_INPUT */

  /* Just about the only way for this code to be executed is if something
     like `bash -i /dev/stdin' is executed. */
  if (interactive_shell && fd_is_tty)
    {
      dup2 (fd, 0);
      close (fd);
      fd = 0;
#if defined (BUFFERED_INPUT)
      default_buffered_input = 0;
#else
      fclose (default_input);
      default_input = stdin;
#endif
    }
  else if (forced_interactive && fd_is_tty == 0)
    /* But if a script is called with something like `bash -i scriptname',
       we need to do a non-interactive setup here, since we didn't do it
       before. */
    init_interactive_script ();

  free (filename);

  reading_shell_script = 1;
  return (fd);
}