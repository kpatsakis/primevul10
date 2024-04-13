test_externally_compressed_io (xd3_stream *stream, int ignore)
{
  usize_t i;
  int ret;
  char buf[TESTBUFSIZE];

  mt_init (& static_mtrand, 0x9f73f7fc);

  if ((ret = test_make_inputs (stream, NULL, NULL))) { return ret; }

  for (i = 0; i < SIZEOF_ARRAY (extcomp_types); i += 1)
    {
      main_extcomp *ext = & extcomp_types[i];

      /* Test for the existence of the external command first, if not skip. */
      snprintf_func (buf, TESTBUFSIZE, "%s %s < /dev/null > /dev/null", ext->recomp_cmdname, ext->recomp_options);

      if ((ret = system (buf)) != 0)
	{
	  XPR(NT "%s=0", ext->recomp_cmdname);
	  continue;
	}

      if ((ret = test_compressed_pipe (stream, ext, buf, "-cfq", "-dcfq", 1,
				       "compression failed: identity pipe")) ||
	  (ret = test_compressed_pipe (stream, ext, buf, "-cfq", "-Rdcfq", 0,
				       "compression failed: without recompression")) ||
	  (ret = test_compressed_pipe (stream, ext, buf, "-Dcfq", "-Rdcfq", 1,
				       "compression failed: without decompression")))
	{
	  return ret;
	}
    }

  return 0;
}