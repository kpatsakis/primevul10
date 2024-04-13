test_command_line_arguments (xd3_stream *stream, int ignore)
{
  int i, ret;

  static const char* cmdpairs[] =
  {
    /* standard input, output */
    "%s %s -A < %s > %s", "%s -d < %s > %s",
    "%s %s -A -e < %s > %s", "%s -d < %s > %s",
    "%s %s -A= encode < %s > %s", "%s decode < %s > %s",
    "%s %s -A -q encode < %s > %s", "%s -qdq < %s > %s",

    /* file input, standard output */
    "%s %s -A= %s > %s", "%s -d %s > %s",
    "%s %s -A -e %s > %s", "%s -d %s > %s",
    "%s %s encode -A= %s > %s", "%s decode %s > %s",

    /* file input, output */
    "%s %s -A= %s %s", "%s -d %s %s",
    "%s %s -A -e %s %s", "%s -d %s %s",
    "%s %s -A= encode %s %s", "%s decode %s %s",

    /* option placement */
    "%s %s -A -f %s %s", "%s -f -d %s %s",
    "%s %s -e -A= %s %s", "%s -d -f %s %s",
    "%s %s -f encode -A= %s %s", "%s -f decode -f %s %s",
  };

  char ecmd[TESTBUFSIZE], dcmd[TESTBUFSIZE];
  int pairs = SIZEOF_ARRAY (cmdpairs) / 2;
  xoff_t tsize;
  xoff_t dsize;
  double ratio;

  mt_init (& static_mtrand, 0x9f73f7fc);

  for (i = 0; i < pairs; i += 1)
    {
      test_setup ();
      if ((ret = test_make_inputs (stream, NULL, & tsize))) { return ret; }

      snprintf_func (ecmd, TESTBUFSIZE, cmdpairs[2*i], program_name,
	       test_softcfg_str, TEST_TARGET_FILE, TEST_DELTA_FILE);
      snprintf_func (dcmd, TESTBUFSIZE, cmdpairs[2*i+1], program_name,
	       TEST_DELTA_FILE, TEST_RECON_FILE);

      /* Encode and decode. */
      if ((ret = system (ecmd)) != 0)
	{
	  XPR(NT "encode command: %s\n", ecmd);
	  stream->msg = "encode cmd failed";
	  return XD3_INTERNAL;
	}

      if ((ret = system (dcmd)) != 0)
	{
	  XPR(NT "decode command: %s\n", dcmd);
	  stream->msg = "decode cmd failed";
	  return XD3_INTERNAL;
	}

      /* Compare the target file. */
      if ((ret = test_compare_files (TEST_TARGET_FILE, TEST_RECON_FILE)))
	{
	  return ret;
	}

      if ((ret = test_file_size (TEST_DELTA_FILE, & dsize)))
	{
	  return ret;
	}

      ratio = (double) dsize / (double) tsize;

      /* Check that it is not too small, not too large. */
      if (ratio >= TEST_ADD_RATIO + TEST_EPSILON)
	{
	  XPR(NT "test encode with size ratio %.4f, "
	     "expected < %.4f (%"Q"u, %"Q"u)\n",
	    ratio, TEST_ADD_RATIO + TEST_EPSILON, dsize, tsize);
	  stream->msg = "strange encoding";
	  return XD3_INTERNAL;
	}

      if (ratio <= TEST_ADD_RATIO * (1.0 - 2 * TEST_EPSILON))
	{
	  XPR(NT "test encode with size ratio %.4f, "
	     "expected > %.4f\n",
	    ratio, TEST_ADD_RATIO - TEST_EPSILON);
	  stream->msg = "strange encoding";
	  return XD3_INTERNAL;
	}

      /* Also check that test_compare_files works.  The delta and original should
       * not be identical. */
      if ((ret = test_compare_files (TEST_DELTA_FILE,
				TEST_TARGET_FILE)) == 0)
	{
	  stream->msg = "broken test_compare_files";
	  return XD3_INTERNAL;
	}

      test_cleanup ();
      DOT ();
    }

  return 0;
}