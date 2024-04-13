main_print_func (xd3_stream* stream, main_file *xfile)
{
  int ret;

  if (option_no_output)
    {
      return 0;
    }

  if (xfile->snprintf_buf == NULL)
    {
      if ((xfile->snprintf_buf =
	   (uint8_t*)main_malloc(SNPRINTF_BUFSIZE)) == NULL)
	{
	  return ENOMEM;
	}
    }

  if (stream->dec_winstart == 0)
    {
      VC(UT "VCDIFF version:               0\n")VE;
      VC(UT "VCDIFF header size:           %d\n",
	 stream->dec_hdrsize)VE;
      VC(UT "VCDIFF header indicator:      ")VE;
      if ((stream->dec_hdr_ind & VCD_SECONDARY) != 0)
	VC(UT "VCD_SECONDARY ")VE;
      if ((stream->dec_hdr_ind & VCD_CODETABLE) != 0)
	VC(UT "VCD_CODETABLE ")VE;
      if ((stream->dec_hdr_ind & VCD_APPHEADER) != 0)
	VC(UT "VCD_APPHEADER ")VE;
      if (stream->dec_hdr_ind == 0)
	VC(UT "none")VE;
      VC(UT "\n")VE;

      IF_SEC(VC(UT "VCDIFF secondary compressor:  %s\n",
		stream->sec_type ? stream->sec_type->name : "none")VE);
      IF_NSEC(VC(UT "VCDIFF secondary compressor: unsupported\n")VE);

      if (stream->dec_hdr_ind & VCD_APPHEADER)
	{
	  uint8_t *apphead;
	  usize_t appheadsz;
	  ret = xd3_get_appheader (stream, & apphead, & appheadsz);

	  if (ret == 0 && appheadsz > 0)
	    {
	      int sq = option_quiet;
	      main_file i, o, s;
	      XD3_ASSERT (apphead != NULL);
	      VC(UT "VCDIFF application header:    ")VE;
	      if ((ret = main_file_write (xfile, apphead,
					  appheadsz, "print")) != 0)
		{ return ret; }
	      VC(UT "\n")VE;

	      main_file_init (& i);
	      main_file_init (& o);
	      main_file_init (& s);
	      option_quiet = 1;
	      main_get_appheader (stream, &i, & o, & s);
	      option_quiet = sq;
	      if ((ret = main_print_vcdiff_file (xfile, & o, "output")))
		{ return ret; }
	      if ((ret = main_print_vcdiff_file (xfile, & s, "source")))
		{ return ret; }
	      main_file_cleanup (& i);
	      main_file_cleanup (& o);
	      main_file_cleanup (& s);
	    }
	}
    }
  else
    {
      VC(UT "\n")VE;
    }

  VC(UT "VCDIFF window number:         %"Q"u\n", stream->current_window)VE;
  VC(UT "VCDIFF window indicator:      ")VE;
  if ((stream->dec_win_ind & VCD_SOURCE) != 0) VC(UT "VCD_SOURCE ")VE;
  if ((stream->dec_win_ind & VCD_TARGET) != 0) VC(UT "VCD_TARGET ")VE;
  if ((stream->dec_win_ind & VCD_ADLER32) != 0) VC(UT "VCD_ADLER32 ")VE;
  if (stream->dec_win_ind == 0) VC(UT "none")VE;
  VC(UT "\n")VE;

  if ((stream->dec_win_ind & VCD_ADLER32) != 0)
    {
      VC(UT "VCDIFF adler32 checksum:      %08X\n",
	 (usize_t)stream->dec_adler32)VE;
    }

  if (stream->dec_del_ind != 0)
    {
      VC(UT "VCDIFF delta indicator:       ")VE;
      if ((stream->dec_del_ind & VCD_DATACOMP) != 0) VC(UT "VCD_DATACOMP ")VE;
      if ((stream->dec_del_ind & VCD_INSTCOMP) != 0) VC(UT "VCD_INSTCOMP ")VE;
      if ((stream->dec_del_ind & VCD_ADDRCOMP) != 0) VC(UT "VCD_ADDRCOMP ")VE;
      if (stream->dec_del_ind == 0) VC(UT "none")VE;
      VC(UT "\n")VE;
    }

  if (stream->dec_winstart != 0)
    {
      VC(UT "VCDIFF window at offset:      %"Q"u\n", stream->dec_winstart)VE;
    }

  if (SRCORTGT (stream->dec_win_ind))
    {
      VC(UT "VCDIFF copy window length:    %u\n",
	 (usize_t)stream->dec_cpylen)VE;
      VC(UT "VCDIFF copy window offset:    %"Q"u\n",
	 stream->dec_cpyoff)VE;
    }

  VC(UT "VCDIFF delta encoding length: %u\n",
     (usize_t)stream->dec_enclen)VE;
  VC(UT "VCDIFF target window length:  %u\n",
     (usize_t)stream->dec_tgtlen)VE;

  VC(UT "VCDIFF data section length:   %u\n",
     (usize_t)stream->data_sect.size)VE;
  VC(UT "VCDIFF inst section length:   %u\n",
     (usize_t)stream->inst_sect.size)VE;
  VC(UT "VCDIFF addr section length:   %u\n",
     (usize_t)stream->addr_sect.size)VE;

  ret = 0;
  if ((stream->flags & XD3_JUST_HDR) != 0)
    {
      /* Print a header -- finished! */
      ret = PRINTHDR_SPECIAL;
    }
  else if ((stream->flags & XD3_SKIP_WINDOW) == 0)
    {
      ret = main_print_window (stream, xfile);
    }

  return ret;
}