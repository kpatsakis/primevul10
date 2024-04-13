test_source_decompression (xd3_stream *stream, int ignore)
{
  int ret;
  char buf[TESTBUFSIZE];
  const main_extcomp *ext;
  xoff_t dsize;

  mt_init (& static_mtrand, 0x9f73f7fc);

  test_setup ();
  if ((ret = test_make_inputs (stream, NULL, NULL))) { return ret; }

  /* Use gzip. */
  if ((ext = main_get_compressor ("G")) == NULL)
    {
      XPR(NT "skipped");
      return 0;
    }

  /* Save an uncompressed copy. */
  if ((ret = test_save_copy (TEST_TARGET_FILE))) { return ret; }

  /* Compress the source. */
  snprintf_func (buf, TESTBUFSIZE, "%s -1 %s < %s > %s", ext->recomp_cmdname,
	   ext->recomp_options, TEST_COPY_FILE, TEST_SOURCE_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }
  /* Compress the target. */
  snprintf_func (buf, TESTBUFSIZE, "%s -9 %s < %s > %s", ext->recomp_cmdname,
	   ext->recomp_options, TEST_COPY_FILE, TEST_TARGET_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  /* Now the two identical files are compressed.  Delta-encode the target,
   * with decompression. */
  snprintf_func (buf, TESTBUFSIZE, "%s -e -vfq -s%s %s %s", program_name, TEST_SOURCE_FILE,
	   TEST_TARGET_FILE, TEST_DELTA_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  /* Check that the compressed file is small (b/c inputs are
   * identical). */
  if ((ret = test_file_size (TEST_DELTA_FILE, & dsize))) { return ret; }
  /* Deltas for identical files should be very small. */
  if (dsize > 200)
    {
      XPR(NT "external compression did not happen\n");
      stream->msg = "external compression did not happen";
      return XD3_INTERNAL;
    }

  /* Decode the delta file with recompression disabled, should get an
   * uncompressed file out. */
  snprintf_func (buf, TESTBUFSIZE, "%s -v -dq -R -s%s %s %s", program_name,
	   TEST_SOURCE_FILE, TEST_DELTA_FILE, TEST_RECON_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }
  if ((ret = test_compare_files (TEST_COPY_FILE,
			    TEST_RECON_FILE))) { return ret; }

  /* Decode the delta file with recompression, should get a compressed file
   * out.  But we can't compare compressed files directly. */
  snprintf_func (buf, TESTBUFSIZE, "%s -v -dqf -s%s %s %s", program_name,
	   TEST_SOURCE_FILE, TEST_DELTA_FILE, TEST_RECON_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }
  snprintf_func (buf, TESTBUFSIZE, "%s %s < %s > %s", ext->decomp_cmdname, ext->decomp_options,
	   TEST_RECON_FILE, TEST_RECON2_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }
  if ((ret = test_compare_files (TEST_COPY_FILE,
			    TEST_RECON2_FILE))) { return ret; }

  /* Encode with decompression disabled */
  snprintf_func (buf, TESTBUFSIZE, "%s -e -D -vfq -s%s %s %s", program_name,
	   TEST_SOURCE_FILE, TEST_TARGET_FILE, TEST_DELTA_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  /* Decode the delta file with decompression disabled, should get the
   * identical compressed file out. */
  snprintf_func (buf, TESTBUFSIZE, "%s -d -D -vfq -s%s %s %s", program_name,
	   TEST_SOURCE_FILE, TEST_DELTA_FILE, TEST_RECON_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }
  if ((ret = test_compare_files (TEST_TARGET_FILE,
			    TEST_RECON_FILE))) { return ret; }

  test_cleanup();
  return 0;
}