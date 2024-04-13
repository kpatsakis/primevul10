main_merge_output (xd3_stream *stream, main_file *ofile)
{
  int ret;
  usize_t inst_pos = 0;
  xoff_t output_pos = 0;
  xd3_source recode_source;
  usize_t window_num = 0;
  int at_least_once = 0;

  /* merge_stream is set if there were arguments.  this stream's input
   * needs to be applied to the merge_stream source. */
  if ((merge_stream != NULL) &&
      (ret = xd3_merge_input_output (stream,
				     & merge_stream->whole_target)))
    {
      XPR(NT XD3_LIB_ERRMSG (stream, ret));
      return ret;
    }

  if (option_use_appheader != 0 &&
      option_appheader != NULL)
    {
      xd3_set_appheader (recode_stream, option_appheader,
			 (usize_t) strlen ((char*) option_appheader));
    }

  /* Enter the ENC_INPUT state and bypass the next_in == NULL test
   * and (leftover) input buffering logic. */
  XD3_ASSERT(recode_stream->enc_state == ENC_INIT);
  recode_stream->enc_state = ENC_INPUT;
  recode_stream->next_in = main_bdata;
  recode_stream->flags |= XD3_FLUSH;

  /* This encodes the entire target. */
  while (inst_pos < stream->whole_target.instlen || !at_least_once)
    {
      xoff_t window_start = output_pos;
      int window_srcset = 0;
      xoff_t window_srcmin = 0;
      xoff_t window_srcmax = 0;
      usize_t window_pos = 0;
      usize_t window_size;

      /* at_least_once ensures that we encode at least one window,
       * which handles the 0-byte case. */
      at_least_once = 1;

      XD3_ASSERT (recode_stream->enc_state == ENC_INPUT);

      if ((ret = xd3_encode_input (recode_stream)) != XD3_WINSTART)
	{
	  XPR(NT "invalid merge state: %s\n", xd3_mainerror (ret));
	  return XD3_INVALID;
	}

      /* Window sizes must match from the input to the output, so that
       * target copies are in-range (and so that checksums carry
       * over). */
      XD3_ASSERT (window_num < stream->whole_target.wininfolen);
      window_size = stream->whole_target.wininfo[window_num].length;

      /* Output position should also match. */
      if (output_pos != stream->whole_target.wininfo[window_num].offset)
	{
	  XPR(NT "internal merge error: offset mismatch\n");
	  return XD3_INVALID;
	}

      if (option_use_checksum &&
	  (stream->dec_win_ind & VCD_ADLER32) != 0)
	{
	  recode_stream->flags |= XD3_ADLER32_RECODE;
	  recode_stream->recode_adler32 =
	    stream->whole_target.wininfo[window_num].adler32;
	}

      window_num++;

      if (main_bsize < window_size)
	{
	  main_buffree (main_bdata);
	  main_bdata = NULL;
	  main_bsize = 0;
	  if ((main_bdata = (uint8_t*)
	       main_bufalloc (window_size)) == NULL)
	    {
	      return ENOMEM;
	    }
	  main_bsize = window_size;
	}

      /* This encodes a single target window. */
      while (window_pos < window_size &&
	     inst_pos < stream->whole_target.instlen)
	{
	  xd3_winst *inst = &stream->whole_target.inst[inst_pos];
	  usize_t take = min(inst->size, window_size - window_pos);
	  xoff_t addr;

	  switch (inst->type)
	    {
	    case XD3_RUN:
	      if ((ret = xd3_emit_run (recode_stream, window_pos, take,
				       &stream->whole_target.adds[inst->addr])))
		{
		  return ret;
		}
	      break;

	    case XD3_ADD:
	      /* Adds are implicit, put them into the input buffer. */
	      memcpy (main_bdata + window_pos,
		      stream->whole_target.adds + inst->addr, take);
	      break;

	    default: /* XD3_COPY + copy mode */
	      if (inst->mode != 0)
		{
		  if (window_srcset) {
		    window_srcmin = min(window_srcmin, inst->addr);
		    window_srcmax = max(window_srcmax, inst->addr + take);
		  } else {
		    window_srcset = 1;
		    window_srcmin = inst->addr;
		    window_srcmax = inst->addr + take;
		  }
		  addr = inst->addr;
		}
	      else
		{
		  XD3_ASSERT (inst->addr >= window_start);
		  addr = inst->addr - window_start;
		}
	      IF_DEBUG2 (XPR(NTR "[merge copy] winpos %u take %u addr %"Q"u mode %u\n",
			    window_pos, take, addr, inst->mode));
	      if ((ret = xd3_found_match (recode_stream, window_pos, take,
					  addr, inst->mode != 0)))
		{
		  return ret;
		}
	      break;
	    }

	  window_pos += take;
	  output_pos += take;

	  if (take == inst->size)
	    {
	      inst_pos += 1;
	    }
	  else
	    {
	      /* Modify the instruction for the next pass. */
	      if (inst->type != XD3_RUN)
		{
		  inst->addr += take;
		}
	      inst->size -= take;
	    }
	}

      xd3_avail_input (recode_stream, main_bdata, window_pos);

      recode_stream->enc_state = ENC_INSTR;

      if (window_srcset) {
	recode_stream->srcwin_decided = 1;
	recode_stream->src = &recode_source;
	recode_source.srclen = (usize_t)(window_srcmax - window_srcmin);
	recode_source.srcbase = window_srcmin;
	recode_stream->taroff = recode_source.srclen;

	XD3_ASSERT (recode_source.srclen != 0);
      } else {
	recode_stream->srcwin_decided = 0;
	recode_stream->src = NULL;
	recode_stream->taroff = 0;
      }

      for (;;)
	{
	  switch ((ret = xd3_encode_input (recode_stream)))
	    {
	    case XD3_INPUT: {
	      goto done_window;
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

	  if ((ret = main_write_output(recode_stream, ofile)))
	    {
	      return ret;
	    }

	  xd3_consume_output (recode_stream);
	}
    done_window:
      (void) 0;
    }

  return 0;
}