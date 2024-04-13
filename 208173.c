main_input (xd3_cmd     cmd,
	    main_file   *ifile,
	    main_file   *ofile,
	    main_file   *sfile)
{
  int        ret;
  xd3_stream stream;
  size_t     nread = 0;
  usize_t    winsize;
  int        stream_flags = 0;
  xd3_config config;
  xd3_source source;
  xoff_t     last_total_in = 0;
  xoff_t     last_total_out = 0;
  long       start_time;
  int        stdout_only = 0;
  int (*input_func) (xd3_stream*);
  int (*output_func) (xd3_stream*, main_file *);

  memset (& stream, 0, sizeof (stream));
  memset (& source, 0, sizeof (source));
  memset (& config, 0, sizeof (config));

  config.alloc = main_alloc;
  config.freef = main_free1;

  config.iopt_size = option_iopt_size;
  config.sprevsz = option_sprevsz;

  do_src_fifo = 0;

  start_time = get_millisecs_now ();

  if (option_use_checksum) { stream_flags |= XD3_ADLER32; }

  /* main_input setup. */
  switch ((int) cmd)
    {
#if VCDIFF_TOOLS
           if (1) { case CMD_PRINTHDR:   stream_flags |= XD3_JUST_HDR; }
      else if (1) { case CMD_PRINTHDRS:  stream_flags |= XD3_SKIP_WINDOW; }
      else        { case CMD_PRINTDELTA: stream_flags |= XD3_SKIP_EMIT; }
      ifile->flags |= RD_NONEXTERNAL;
      input_func    = xd3_decode_input;
      output_func   = main_print_func;
      stream_flags |= XD3_ADLER32_NOVER;
      stdout_only   = 1;
      break;

    case CMD_RECODE:
    case CMD_MERGE:
    case CMD_MERGE_ARG:
      /* No source will be read */
      stream_flags |= XD3_ADLER32_NOVER | XD3_SKIP_EMIT;
      ifile->flags |= RD_NONEXTERNAL;
      input_func = xd3_decode_input;

      if ((ret = main_init_recode_stream ()))
        {
	  return EXIT_FAILURE;
        }

      if (cmd == CMD_RECODE) { output_func = main_recode_func; }
      else                   { output_func = main_merge_func; }
      break;
#endif /* VCDIFF_TOOLS */

#if XD3_ENCODER
    case CMD_ENCODE:
      do_src_fifo = 1;
      input_func  = xd3_encode_input;
      output_func = main_write_output;

      if (option_no_compress)      { stream_flags |= XD3_NOCOMPRESS; }
      if (option_use_altcodetable) { stream_flags |= XD3_ALT_CODE_TABLE; }
      if (option_smatch_config)
	{
	  const char *s = option_smatch_config;
	  char *e;
	  int values[XD3_SOFTCFG_VARCNT];
	  int got;

	  config.smatch_cfg = XD3_SMATCH_SOFT;

	  for (got = 0; got < XD3_SOFTCFG_VARCNT; got += 1, s = e + 1)
	    {
	      values[got] = strtol (s, &e, 10);

	      if ((values[got] < 0) ||
		  (e == s) ||
		  (got < XD3_SOFTCFG_VARCNT-1 && *e == 0) ||
		  (got == XD3_SOFTCFG_VARCNT-1 && *e != 0))
		{
		  XPR(NT "invalid string match specifier (-C) %d: %s\n",
		      got, s);
		  return EXIT_FAILURE;
		}
	    }

	  config.smatcher_soft.large_look    = values[0];
	  config.smatcher_soft.large_step    = values[1];
	  config.smatcher_soft.small_look    = values[2];
	  config.smatcher_soft.small_chain   = values[3];
	  config.smatcher_soft.small_lchain  = values[4];
	  config.smatcher_soft.max_lazy      = values[5];
	  config.smatcher_soft.long_enough   = values[6];
	}
      else
	{
	  if (option_verbose > 2)
	    {
	      XPR(NT "compression level: %d\n", option_level);
	    }
	  if (option_level == 0)
	    {
	      stream_flags |= XD3_NOCOMPRESS;
	      config.smatch_cfg = XD3_SMATCH_FASTEST;
	    }
	  else if (option_level == 1)
	    { config.smatch_cfg = XD3_SMATCH_FASTEST; }
	  else if (option_level == 2)
	    { config.smatch_cfg = XD3_SMATCH_FASTER; }
	  else if (option_level <= 5)
	    { config.smatch_cfg = XD3_SMATCH_FAST; }
	  else if (option_level == 6)
	    { config.smatch_cfg = XD3_SMATCH_DEFAULT; }
	  else
	    { config.smatch_cfg = XD3_SMATCH_SLOW; }
	}
      break;
#endif
    case CMD_DECODE:
      if (option_use_checksum == 0) { stream_flags |= XD3_ADLER32_NOVER; }
      ifile->flags |= RD_NONEXTERNAL;
      input_func    = xd3_decode_input;
      output_func   = main_write_output;
      break;
    default:
      XPR(NT "internal error\n");
      return EXIT_FAILURE;
    }

  main_bsize = winsize = main_get_winsize (ifile);

  if ((main_bdata = (uint8_t*) main_bufalloc (winsize)) == NULL)
    {
      return EXIT_FAILURE;
    }

  config.winsize = winsize;
  config.getblk = main_getblk_func;
  config.flags = stream_flags;

  if ((ret = main_set_secondary_flags (&config)) ||
      (ret = xd3_config_stream (& stream, & config)))
    {
      XPR(NT XD3_LIB_ERRMSG (& stream, ret));
      return EXIT_FAILURE;
    }

#if VCDIFF_TOOLS
  if ((cmd == CMD_MERGE || cmd == CMD_MERGE_ARG) &&
      (ret = xd3_whole_state_init (& stream)))
    {
      XPR(NT XD3_LIB_ERRMSG (& stream, ret));
      return EXIT_FAILURE;
    }
#endif

  if (cmd != CMD_DECODE)
    {
      /* When not decoding, set source now.  The decoder delays this
       * step until XD3_GOTHEADER. */
      if (sfile && sfile->filename != NULL)
	{
	  if ((ret = main_set_source (& stream, cmd, sfile, & source)))
	    {
	      return EXIT_FAILURE;
	    }

	  XD3_ASSERT(stream.src != NULL);
	}
    }

  if (cmd == CMD_PRINTHDR ||
      cmd == CMD_PRINTHDRS ||
      cmd == CMD_PRINTDELTA ||
      cmd == CMD_RECODE)
    {
      if (sfile->filename == NULL)
	{
	  allow_fake_source = 1;
	  sfile->filename = "<placeholder>";
	  main_set_source (& stream, cmd, sfile, & source);
	}
    }

  /* This times each window. */
  get_millisecs_since ();

  /* Main input loop. */
  do
    {
      xoff_t input_offset;
      xoff_t input_remain;
      usize_t try_read;

      input_offset = ifile->nread;

      input_remain = XOFF_T_MAX - input_offset;

      try_read = (usize_t) min ((xoff_t) config.winsize, input_remain);

      if ((ret = main_read_primary_input (ifile, main_bdata,
					  try_read, & nread)))
	{
	  return EXIT_FAILURE;
	}

      /* If we've reached EOF tell the stream to flush. */
      if (nread < try_read)
	{
	  stream.flags |= XD3_FLUSH;
	}

#if XD3_ENCODER
      /* After the first main_read_primary_input completes, we know
       * all the information needed to encode the application
       * header. */
      if (cmd == CMD_ENCODE &&
	  (ret = main_set_appheader (& stream, ifile, sfile)))
	{
	  return EXIT_FAILURE;
	}
#endif
      xd3_avail_input (& stream, main_bdata, nread);

      /* If we read zero bytes after encoding at least one window... */
      if (nread == 0 && stream.current_window > 0) {
	break;
      }

    again:
      ret = input_func (& stream);

      switch (ret)
	{
	case XD3_INPUT:
	  continue;

	case XD3_GOTHEADER:
	  {
	    XD3_ASSERT (stream.current_window == 0);

	    /* Need to process the appheader as soon as possible.  It may
	     * contain a suggested default filename/decompression routine for
	     * the ofile, and it may contain default/decompression routine for
	     * the sources. */
	    if (cmd == CMD_DECODE)
	      {
		/* May need to set the sfile->filename if none was given. */
		main_get_appheader (& stream, ifile, ofile, sfile);

		/* Now open the source file. */
		  if ((sfile->filename != NULL) &&
		      (ret = main_set_source (& stream, cmd, sfile, & source)))
		  {
		    return EXIT_FAILURE;
		  }
	      }
	  }
	/* FALLTHROUGH */
	case XD3_WINSTART:
	  {
	    /* e.g., set or unset XD3_SKIP_WINDOW. */
	    goto again;
	  }

	case XD3_OUTPUT:
	  {
	    /* Defer opening the output file until the stream produces its
	     * first output for both encoder and decoder, this way we
	     * delay long enough for the decoder to receive the
	     * application header.  (Or longer if there are skipped
	     * windows, but I can't think of any reason not to delay
	     * open.) */
	    if (ofile != NULL &&
		! main_file_isopen (ofile) &&
		(ret = main_open_output (& stream, ofile)) != 0)
	      {
		return EXIT_FAILURE;
	      }

	    if ((ret = output_func (& stream, ofile)) &&
		(ret != PRINTHDR_SPECIAL))
	      {
		return EXIT_FAILURE;
	      }

	    if (ret == PRINTHDR_SPECIAL)
	      {
		xd3_abort_stream (& stream);
		ret = EXIT_SUCCESS;
		goto done;
	      }

	    ret = 0;

	    xd3_consume_output (& stream);
	    goto again;
	  }

	case XD3_WINFINISH:
	  {
	    if (IS_ENCODE (cmd) || cmd == CMD_DECODE || cmd == CMD_RECODE)
	      {
		if (! option_quiet && IS_ENCODE (cmd) &&
		    main_file_isopen (sfile))
		  {
		    /* Warn when no source copies are found */
		    if (option_verbose && ! xd3_encoder_used_source (& stream))
		      {
			XPR(NT "warning: input window %"Q"u..%"Q"u has "
			    "no source copies\n",
			    stream.current_window * winsize,
			    (stream.current_window+1) * winsize);
			XD3_ASSERT (stream.src != NULL);
		      }

		    /* Limited i-buffer size affects source copies
		     * when the sourcewin is decided early. */
		    if (option_verbose > 1 &&
			stream.srcwin_decided_early &&
			stream.i_slots_used > stream.iopt_size)
		      {
			XPR(NT "warning: input position %"Q"u overflowed "
			    "instruction buffer, needed %u (vs. %u), "
			    "consider changing -I\n",
			    stream.current_window * winsize,
			    stream.i_slots_used, stream.iopt_size);
		      }
		  }

		if (option_verbose)
		  {
		    shortbuf rrateavg, wrateavg, tm;
		    shortbuf rdb, wdb;
		    shortbuf trdb, twdb;
		    shortbuf srcpos;
		    long millis = get_millisecs_since ();
		    usize_t this_read = (usize_t)(stream.total_in -
						  last_total_in);
		    usize_t this_write = (usize_t)(stream.total_out -
						   last_total_out);
		    last_total_in = stream.total_in;
		    last_total_out = stream.total_out;

		    if (option_verbose > 1)
		      {
			XPR(NT "%"Q"u: in %s (%s): out %s (%s): "
			    "total in %s: out %s: %s: srcpos %s\n",
			    stream.current_window,
			    main_format_bcnt (this_read, &rdb),
			    main_format_rate (this_read, millis, &rrateavg),
			    main_format_bcnt (this_write, &wdb),
			    main_format_rate (this_write, millis, &wrateavg),
			    main_format_bcnt (stream.total_in, &trdb),
			    main_format_bcnt (stream.total_out, &twdb),
			    main_format_millis (millis, &tm),
			    main_format_bcnt (sfile->source_position, &srcpos));
		      }
		    else
		      {
			XPR(NT "%"Q"u: in %s: out %s: total in %s: "
			    "out %s: %s\n",
 			    stream.current_window,
			    main_format_bcnt (this_read, &rdb),
			    main_format_bcnt (this_write, &wdb),
			    main_format_bcnt (stream.total_in, &trdb),
			    main_format_bcnt (stream.total_out, &twdb),
			    main_format_millis (millis, &tm));
		      }
		  }
	      }
	    goto again;
	  }

	default:
	  /* input_func() error */
	  XPR(NT XD3_LIB_ERRMSG (& stream, ret));
	  if (! option_quiet && ret == XD3_INVALID_INPUT)
	    {
	      XPR(NT "normally this indicates that the source file is incorrect\n");
	      XPR(NT "please verify the source file with sha1sum or equivalent\n");
	    }
	  return EXIT_FAILURE;
	}
    }
  while (nread == config.winsize);
done:
  /* Close the inputs. (ifile must be open, sfile may be open) */
  main_file_close (ifile);
  if (sfile != NULL)
    {
      main_file_close (sfile);
    }

#if VCDIFF_TOOLS
  if (cmd == CMD_MERGE &&
      (ret = main_merge_output (& stream, ofile)))
    {
      return EXIT_FAILURE;
    }

  if (cmd == CMD_MERGE_ARG)
    {
      xd3_swap_whole_state (& stream.whole_target,
			    & recode_stream->whole_target);
    }
#endif /* VCDIFF_TOOLS */

  /* If output file is not open yet because of delayed-open, it means
   * we never encountered a window in the delta, but it could have had
   * a VCDIFF header?  TODO: solve this elsewhere.  For now, it prints
   * "nothing to output" below, but the check doesn't happen in case
   * of option_no_output.  */
  if (! option_no_output && ofile != NULL)
    {
      if (!stdout_only && ! main_file_isopen (ofile))
	{
	  XPR(NT "nothing to output: %s\n", ifile->filename);
	  return EXIT_FAILURE;
	}

      /* Have to close the output before calling
       * main_external_compression_finish, or else it hangs. */
      if (main_file_close (ofile) != 0)
	{
	  return EXIT_FAILURE;
	}
    }

#if EXTERNAL_COMPRESSION
  if ((ret = main_external_compression_finish ()))
    {
      XPR(NT "external compression commands failed\n");
      return EXIT_FAILURE;
    }
#endif

  if ((ret = xd3_close_stream (& stream)))
    {
      XPR(NT XD3_LIB_ERRMSG (& stream, ret));
      return EXIT_FAILURE;
    }

#if XD3_ENCODER
  if (option_verbose > 1 && cmd == CMD_ENCODE)
    {
      XPR(NT "scanner configuration: %s\n", stream.smatcher.name);
      XPR(NT "target hash table size: %u\n", stream.small_hash.size);
      if (sfile != NULL && sfile->filename != NULL)
	{
	  XPR(NT "source hash table size: %u\n", stream.large_hash.size);
	}
    }

  if (option_verbose > 2 && cmd == CMD_ENCODE)
    {
      XPR(NT "source copies: %"Q"u (%"Q"u bytes)\n",
	  stream.n_scpy, stream.l_scpy);
      XPR(NT "target copies: %"Q"u (%"Q"u bytes)\n",
	  stream.n_tcpy, stream.l_tcpy);
      XPR(NT "adds: %"Q"u (%"Q"u bytes)\n", stream.n_add, stream.l_add);
      XPR(NT "runs: %"Q"u (%"Q"u bytes)\n", stream.n_run, stream.l_run);
    }
#endif

  xd3_free_stream (& stream);

  if (option_verbose)
    {
      shortbuf tm;
      long end_time = get_millisecs_now ();
      xoff_t nwrite = ofile != NULL ? ofile->nwrite : 0;

      XPR(NT "finished in %s; input %"Q"u output %"Q"u bytes (%0.2f%%)\n",
	  main_format_millis (end_time - start_time, &tm),
	  ifile->nread, nwrite, 100.0 * nwrite / ifile->nread);
    }

  return EXIT_SUCCESS;
}