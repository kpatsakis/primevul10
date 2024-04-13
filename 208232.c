test_compressed_pipe (xd3_stream *stream, main_extcomp *ext, char* buf,
		      const char* comp_options, const char* decomp_options,
		      int do_ext_recomp, const char* msg)
{
  int ret;
  char decomp_buf[TESTBUFSIZE];

  if (do_ext_recomp)
    {
      snprintf_func (decomp_buf, TESTBUFSIZE,
		" | %s %s", ext->decomp_cmdname, ext->decomp_options);
    }
  else
    {
      decomp_buf[0] = 0;
    }

  snprintf_func (buf, TESTBUFSIZE, "%s %s < %s | %s %s | %s %s%s > %s",
	   ext->recomp_cmdname, ext->recomp_options,
	   TEST_TARGET_FILE,
	   program_name, comp_options,
	   program_name, decomp_options,
	   decomp_buf,
	   TEST_RECON_FILE);

  if ((ret = system (buf)) != 0)
    {
      stream->msg = msg;
      return XD3_INTERNAL;
    }

  if ((ret = test_compare_files (TEST_TARGET_FILE, TEST_RECON_FILE)))
    {
      return XD3_INTERNAL;
    }

  DOT ();
  return 0;
}