test_streaming (xd3_stream *in_stream, uint8_t *encbuf, uint8_t *decbuf, uint8_t *delbuf, usize_t megs)
{
  xd3_stream estream, dstream;
  int ret;
  usize_t i, delsize, decsize;
  xd3_config cfg;
  xd3_init_config (& cfg, in_stream->flags);
  cfg.flags |= XD3_COMPLEVEL_6;

  if ((ret = xd3_config_stream (& estream, & cfg)) ||
      (ret = xd3_config_stream (& dstream, & cfg)))
    {
      goto fail;
    }

  for (i = 0; i < megs; i += 1)
    {
      ((usize_t*) encbuf)[0] = i;

      if ((i % 200) == 199) { DOT (); }

      if ((ret = xd3_process_stream (1, & estream, xd3_encode_input, 0,
				     encbuf, 1 << 20,
				     delbuf, & delsize, 1 << 20)))
	{
	  in_stream->msg = estream.msg;
	  goto fail;
	}

      if ((ret = xd3_process_stream (0, & dstream, xd3_decode_input, 0,
				     delbuf, delsize,
				     decbuf, & decsize, 1 << 20)))
	{
	  in_stream->msg = dstream.msg;
	  goto fail;
	}

      if (decsize != 1 << 20 ||
	  memcmp (encbuf, decbuf, 1 << 20) != 0)
	{
	  in_stream->msg = "wrong result";
	  ret = XD3_INTERNAL;
	  goto fail;
	}
    }

  if ((ret = xd3_close_stream (& estream)) ||
      (ret = xd3_close_stream (& dstream)))
    {
      goto fail;
    }

 fail:
  xd3_free_stream (& estream);
  xd3_free_stream (& dstream);
  return ret;
}