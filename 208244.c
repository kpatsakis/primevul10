main_init_recode_stream (void)
{
  int ret;
  int stream_flags = XD3_ADLER32_NOVER | XD3_SKIP_EMIT;
  int recode_flags;
  xd3_config recode_config;

  XD3_ASSERT (recode_stream == NULL);

  if ((recode_stream = (xd3_stream*) main_malloc(sizeof(xd3_stream))) == NULL)
    {
      return ENOMEM;
    }

  recode_flags = (stream_flags & XD3_SEC_TYPE);

  recode_config.alloc = main_alloc;
  recode_config.freef = main_free1;

  xd3_init_config(&recode_config, recode_flags);

  if ((ret = main_set_secondary_flags (&recode_config)) ||
      (ret = xd3_config_stream (recode_stream, &recode_config)) ||
      (ret = xd3_encode_init_partial (recode_stream)) ||
      (ret = xd3_whole_state_init (recode_stream)))
    {
      XPR(NT XD3_LIB_ERRMSG (recode_stream, ret));
      xd3_free_stream (recode_stream);
      recode_stream = NULL;
      return ret;
    }

  return 0;
}