test_force_behavior (xd3_stream *stream, int ignore)
{
  int ret;
  char buf[TESTBUFSIZE];

  /* Create empty target file */
  test_setup ();
  snprintf_func (buf, TESTBUFSIZE, "cp /dev/null %s", TEST_TARGET_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  /* Encode to delta file */
  snprintf_func (buf, TESTBUFSIZE, "%s -e %s %s", program_name,
	   TEST_TARGET_FILE, TEST_DELTA_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  /* Encode again, should fail. */
  snprintf_func (buf, TESTBUFSIZE, "%s -q -e %s %s ", program_name,
	   TEST_TARGET_FILE, TEST_DELTA_FILE);
  if ((ret = do_fail (stream, buf))) { return ret; }

  /* Force it, should succeed. */
  snprintf_func (buf, TESTBUFSIZE, "%s -f -e %s %s", program_name,
	   TEST_TARGET_FILE, TEST_DELTA_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }
  test_cleanup();
  return 0;
}