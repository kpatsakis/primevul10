main_secondary_decompress_check (main_file  *file,
				 uint8_t    *input_buf,
				 size_t      input_size,
				 size_t     *nread)
{
  int ret;
  usize_t i;
  usize_t try_read = min (input_size, XD3_ALLOCSIZE);
  size_t  check_nread = 0;
  uint8_t check_buf[XD3_ALLOCSIZE];  /* TODO: stack limit */
  const main_extcomp *decompressor = NULL;

  if ((ret = main_file_read (file, check_buf,
			     try_read,
			     & check_nread, "input read failed")))
    {
      return ret;
    }

  if (file->flags & RD_DECOMPSET)
    {
      /* This allows the application header to override the magic
       * number, for whatever reason. */
      decompressor = file->compressor;
    }
  else
    {
      for (i = 0; i < SIZEOF_ARRAY (extcomp_types); i += 1)
	{
	  const main_extcomp *decomp = & extcomp_types[i];

	  if (check_nread > decomp->magic_size)
	    {
	      /* The following expr checks if we are trying to read a
	       * VCDIFF input, in which case do not treat it as
	       * "secondary" decompression. */
	      int skip_this_type = (decomp->flags & RD_NONEXTERNAL) &&
  	                           (file->flags & RD_NONEXTERNAL);

	      if (skip_this_type)
		{
		  continue;
		}

	      if (memcmp (check_buf, decomp->magic, decomp->magic_size) == 0)
		{
		  decompressor = decomp;
		  break;
		}
	    }
	}
    }

  if (decompressor != NULL)
    {
      if (! option_quiet)
	{
	  XPR(NT "externally compressed input: %s %s%s < %s\n",
	      decompressor->decomp_cmdname,
	      decompressor->decomp_options,
	      (option_force2 ? " -f" : ""),
	      file->filename);
	  if (file->flags & RD_MAININPUT)
	    {
	      XPR(NT
  "WARNING: the encoder is automatically decompressing the input file;\n");
	      XPR(NT
  "WARNING: the decoder will automatically recompress the output file;\n");
	      XPR(NT
  "WARNING: this may result in different compressed data and checksums\n");
	      XPR(NT
  "WARNING: despite being identical data; if this is an issue, use -D\n");
	      XPR(NT
  "WARNING: to avoid decompression and/or use -R to avoid recompression\n");
	      XPR(NT
  "WARNING: and/or manually decompress the input file; if you know the\n");
	      XPR(NT
  "WARNING: compression settings that will produce identical output\n");
	      XPR(NT
  "WARNING: you may set those flags using the environment (e.g., GZIP=-9)\n");
	    }
	}

      file->size_known = 0;
      return main_input_decompress_setup (decompressor, file,
					  input_buf, input_size,
					  check_buf, XD3_ALLOCSIZE,
					  check_nread, nread);
    }

  /* Now read the rest of the input block. */
  (*nread) = 0;

  if (check_nread == try_read)
    {
      ret = main_file_read (file,
			    input_buf + try_read,
			    input_size - try_read,
			    nread,
			    "input read failed");
    }

  memcpy (input_buf, check_buf, check_nread);

  (*nread) += check_nread;

  return 0;
}