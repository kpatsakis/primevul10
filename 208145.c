test_compress_text (xd3_stream  *stream,
		    uint8_t     *encoded,
		    usize_t     *encoded_size)
{
  int ret;
  xd3_config cfg;
  int oflags = stream->flags;
  int flags = stream->flags | XD3_FLUSH;

  xd3_free_stream (stream);
  xd3_init_config (& cfg, flags);

  /* This configuration is fixed so that the "expected non-error" the counts in
   * decompress_single_bit_errors are too.  See test_coftcfg_str. */
  cfg.smatch_cfg = XD3_SMATCH_SOFT;
  cfg.smatcher_soft.name = "test";
  cfg.smatcher_soft.large_look = 64; /* no source, not used */
  cfg.smatcher_soft.large_step = 64; /* no source, not used */
  cfg.smatcher_soft.small_look = 4;
  cfg.smatcher_soft.small_chain = 128;
  cfg.smatcher_soft.small_lchain = 16;
  cfg.smatcher_soft.max_lazy = 8;
  cfg.smatcher_soft.long_enough = 128;

  xd3_config_stream (stream, & cfg);

  (*encoded_size) = 0;

  xd3_set_appheader (stream, test_apphead,
		     (usize_t) strlen ((char*) test_apphead));

  if ((ret = xd3_encode_stream (stream, test_text, sizeof (test_text),
				encoded, encoded_size, 4*sizeof (test_text)))) { goto fail; }

  if ((ret = xd3_close_stream (stream))) { goto fail; }

 fail:
  xd3_free_stream (stream);
  xd3_init_config (& cfg, oflags);
  xd3_config_stream (stream, & cfg);
  return ret;
}