main_open_output (xd3_stream *stream, main_file *ofile)
{
  int ret;

  if (option_no_output)
    {
      return 0;
    }

  if (ofile->filename == NULL)
    {
      XSTDOUT_XF (ofile);

      if (option_verbose > 1)
	{
	  XPR(NT "using standard output: %s\n", ofile->filename);
	}
    }
  else
    {
      /* Stat the file to check for overwrite. */
      if (option_force == 0 && main_file_exists (ofile))
	{
	  if (!option_quiet)
	    {
	      XPR(NT "to overwrite output file specify -f: %s\n",
		  ofile->filename);
	    }
	  return EEXIST;
	}

      if ((ret = main_file_open (ofile, ofile->filename, XO_WRITE)))
	{
	  return ret;
	}

      if (option_verbose > 1) { XPR(NT "output %s\n", ofile->filename); }
    }

#if EXTERNAL_COMPRESSION
  /* Do output recompression. */
  if (ofile->compressor != NULL && option_recompress_outputs == 1)
    {
      if (! option_quiet)
	{
	  XPR(NT "externally compressed output: %s %s%s > %s\n",
	      ofile->compressor->recomp_cmdname,
	      ofile->compressor->recomp_options,
	      (option_force2 ? " -f" : ""),
	      ofile->filename);
	}

      if ((ret = main_recompress_output (ofile)))
	{
	  return ret;
	}
    }
#endif

  return 0;
}