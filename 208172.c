main_recode_func (xd3_stream* stream, main_file *ofile)
{
  int ret;
  xd3_source decode_source;

  XD3_ASSERT(stream->dec_state == DEC_FINISH);
  XD3_ASSERT(recode_stream->enc_state == ENC_INIT ||
	     recode_stream->enc_state == ENC_INPUT);

  // Copy partial decoder output to partial encoder inputs
  if ((ret = main_recode_copy (recode_stream,
			       DATA_HEAD(recode_stream),
			       &stream->data_sect)) ||
      (ret = main_recode_copy (recode_stream,
			       INST_HEAD(recode_stream),
			       &stream->inst_sect)) ||
      (ret = main_recode_copy (recode_stream,
			       ADDR_HEAD(recode_stream),
			       &stream->addr_sect)))
    {
      return ret;
    }

  // This jumps to xd3_emit_hdr()
  recode_stream->enc_state = ENC_FLUSH;
  recode_stream->avail_in = stream->dec_tgtlen;

  if (SRCORTGT (stream->dec_win_ind))
    {
      recode_stream->src = & decode_source;
      decode_source.srclen = stream->dec_cpylen;
      decode_source.srcbase = stream->dec_cpyoff;
    }

  if (option_use_checksum &&
      (stream->dec_win_ind & VCD_ADLER32) != 0)
    {
      recode_stream->flags |= XD3_ADLER32_RECODE;
      recode_stream->recode_adler32 = stream->dec_adler32;
    }

  if (option_use_appheader != 0 &&
      option_appheader != NULL)
    {
      xd3_set_appheader (recode_stream, option_appheader,
			 (usize_t) strlen ((char*) option_appheader));
    }
  else if (option_use_appheader != 0 &&
	   option_appheader == NULL)
    {
      if (stream->dec_appheader != NULL)
	{
	  xd3_set_appheader (recode_stream,
			     stream->dec_appheader, stream->dec_appheadsz);
	}
    }

  // Output loop
  for (;;)
    {
      switch((ret = xd3_encode_input (recode_stream)))
	{
	case XD3_INPUT: {
	  /* finished recoding one window */
	  stream->total_out = recode_stream->total_out;
	  return 0;
	}
	case XD3_OUTPUT: {
	  /* main_file_write below */
	  break;
	}
	case XD3_GOTHEADER:
	case XD3_WINSTART:
	case XD3_WINFINISH: {
	  /* ignore */
	  continue;
	}
	case XD3_GETSRCBLK:
	case 0: {
	    return XD3_INTERNAL;
	  }
	default:
	  return ret;
	}

      if ((ret = main_write_output (recode_stream, ofile)))
	{
	  return ret;
	}

      xd3_consume_output (recode_stream);
    }
}