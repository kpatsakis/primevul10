test_iopt_flush_instructions (xd3_stream *stream, int ignore)
{
  int ret, i;
  usize_t tpos = 0;
  usize_t delta_size, recon_size;
  xd3_config config;
  uint8_t target[TESTBUFSIZE];
  uint8_t delta[TESTBUFSIZE];
  uint8_t recon[TESTBUFSIZE];

  xd3_free_stream (stream);
  xd3_init_config (& config, 0);

  config.smatch_cfg    = XD3_SMATCH_SOFT;
  config.smatcher_soft.large_look    = 16;
  config.smatcher_soft.large_step    = 16;
  config.smatcher_soft.small_look    = 4;
  config.smatcher_soft.small_chain   = 128;
  config.smatcher_soft.small_lchain  = 16;
  config.smatcher_soft.max_lazy      = 8;
  config.smatcher_soft.long_enough   = 128;

  if ((ret = xd3_config_stream (stream, & config))) { return ret; }

  for (i = 1; i < 250; i++)
    {
      target[tpos++] = i;
      target[tpos++] = i+1;
      target[tpos++] = i+2;
      target[tpos++] = i+3;
      target[tpos++] = 0;
    }
  for (i = 1; i < 253; i++)
    {
      target[tpos++] = i;
    }

  if ((ret = xd3_encode_stream (stream, target, tpos,
				    delta, & delta_size, sizeof (delta))))
    {
      return ret;
    }

  xd3_free_stream(stream);
  if ((ret = xd3_config_stream (stream, & config))) { return ret; }

  if ((ret = xd3_decode_stream (stream, delta, delta_size,
				recon, & recon_size, sizeof (recon))))
    {
      return ret;
    }

  CHECK(tpos == recon_size);
  CHECK(memcmp(target, recon, recon_size) == 0);

  return 0;
}