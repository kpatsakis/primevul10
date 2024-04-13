test_stdout_behavior (xd3_stream *stream, int ignore)
{
  int ret;
  char buf[TESTBUFSIZE];

  test_setup();
  snprintf_func (buf, TESTBUFSIZE, "cp /dev/null %s", TEST_TARGET_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  /* Without -c, encode writes to delta file */
  snprintf_func (buf, TESTBUFSIZE, "%s -e %s %s", program_name,
	   TEST_TARGET_FILE, TEST_DELTA_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  /* With -c, encode writes to stdout */
  snprintf_func (buf, TESTBUFSIZE, "%s -e -c %s > %s", program_name,
	   TEST_TARGET_FILE, TEST_DELTA_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  /* Without -c, decode writes to target file name, but it fails because the
   * file exists. */
  snprintf_func (buf, TESTBUFSIZE, "%s -q -d %s ", program_name, TEST_DELTA_FILE);
  if ((ret = do_fail (stream, buf))) { return ret; }

  /* With -c, decode writes to stdout */
  snprintf_func (buf, TESTBUFSIZE, "%s -d -c %s > /dev/null", program_name, TEST_DELTA_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }
  test_cleanup();

  return 0;
}