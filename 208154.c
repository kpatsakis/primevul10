main_print_window (xd3_stream* stream, main_file *xfile)
{
  int ret;
  usize_t size = 0;

  VC(UT "  Offset Code Type1 Size1  @Addr1 + Type2 Size2 @Addr2\n")VE;

  while (stream->inst_sect.buf < stream->inst_sect.buf_max)
    {
      usize_t code = stream->inst_sect.buf[0];
      const uint8_t *addr_before = stream->addr_sect.buf;
      const uint8_t *inst_before = stream->inst_sect.buf;
      usize_t addr_bytes;
      usize_t inst_bytes;
      usize_t size_before = size;

      if ((ret = xd3_decode_instruction (stream)))
	{
	  XPR(NT "instruction decode error at %"Q"u: %s\n",
	      stream->dec_winstart + size, stream->msg);
	  return ret;
	}

      addr_bytes = (usize_t)(stream->addr_sect.buf - addr_before);
      inst_bytes = (usize_t)(stream->inst_sect.buf - inst_before);

      VC(UT "  %06"Q"u %03u  %s %6u", stream->dec_winstart + size,
	 option_print_cpymode ? code : 0,
	 xd3_rtype_to_string ((xd3_rtype) stream->dec_current1.type,
			      option_print_cpymode),
	 stream->dec_current1.size)VE;

      if (stream->dec_current1.type != XD3_NOOP)
	{
	  if (stream->dec_current1.type >= XD3_CPY)
	    {
	      if (stream->dec_current1.addr >= stream->dec_cpylen)
		{
		  VC(UT " T@%-6u",
		     stream->dec_current1.addr - stream->dec_cpylen)VE;
		}
	      else
		{
		  VC(UT " S@%-6"Q"u",
		     stream->dec_cpyoff + stream->dec_current1.addr)VE;
		}
	    }
	  else
	    {
	      VC(UT "        ")VE;
	    }

	  size += stream->dec_current1.size;
	}

      if (stream->dec_current2.type != XD3_NOOP)
	{
	  VC(UT "  %s %6u",
	     xd3_rtype_to_string ((xd3_rtype) stream->dec_current2.type,
				  option_print_cpymode),
	     stream->dec_current2.size)VE;

	  if (stream->dec_current2.type >= XD3_CPY)
	    {
	      if (stream->dec_current2.addr >= stream->dec_cpylen)
		{
		  VC(UT " T@%-6u",
		     stream->dec_current2.addr - stream->dec_cpylen)VE;
		}
	      else
		{
		  VC(UT " S@%-6"Q"u",
		     stream->dec_cpyoff + stream->dec_current2.addr)VE;
		}
	    }

	  size += stream->dec_current2.size;
	}

      VC(UT "\n")VE;

      if (option_verbose &&
	  addr_bytes + inst_bytes >= (size - size_before) &&
	  (stream->dec_current1.type >= XD3_CPY ||
	   stream->dec_current2.type >= XD3_CPY))
	{
	  VC(UT "  %06"Q"u (inefficiency) %u encoded as %u bytes\n",
	     stream->dec_winstart + size_before,
	     size - size_before,
	     addr_bytes + inst_bytes)VE;
	}
    }

  if (stream->dec_tgtlen != size && (stream->flags & XD3_SKIP_WINDOW) == 0)
    {
      XPR(NT "target window size inconsistency");
      return XD3_INTERNAL;
    }

  if (stream->dec_position != stream->dec_maxpos)
    {
      XPR(NT "target window position inconsistency");
      return XD3_INTERNAL;
    }

  if (stream->addr_sect.buf != stream->addr_sect.buf_max)
    {
      XPR(NT "address section inconsistency");
      return XD3_INTERNAL;
    }

  return 0;
}