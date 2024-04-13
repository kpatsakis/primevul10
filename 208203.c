test_no_output (xd3_stream *stream, int ignore)
{
  int ret;
  char buf[TESTBUFSIZE];

  test_setup ();

  snprintf_func (buf, TESTBUFSIZE, "touch %s && chmod 0000 %s",
	   TEST_NOPERM_FILE, TEST_NOPERM_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  if ((ret = test_make_inputs (stream, NULL, NULL))) { return ret; }

  /* Try no_output encode w/out unwritable output file */
  snprintf_func (buf, TESTBUFSIZE, "%s -q -f -e %s %s", program_name,
	   TEST_TARGET_FILE, TEST_NOPERM_FILE);
  if ((ret = do_fail (stream, buf))) { return ret; }
  snprintf_func (buf, TESTBUFSIZE, "%s -J -e %s %s", program_name,
	   TEST_TARGET_FILE, TEST_NOPERM_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  /* Now really write the delta to test decode no-output */
  snprintf_func (buf, TESTBUFSIZE, "%s -e %s %s", program_name,
	   TEST_TARGET_FILE, TEST_DELTA_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }

  snprintf_func (buf, TESTBUFSIZE, "%s -q -f -d %s %s", program_name,
	   TEST_DELTA_FILE, TEST_NOPERM_FILE);
  if ((ret = do_fail (stream, buf))) { return ret; }
  snprintf_func (buf, TESTBUFSIZE, "%s -J -d %s %s", program_name,
	   TEST_DELTA_FILE, TEST_NOPERM_FILE);
  if ((ret = do_cmd (stream, buf))) { return ret; }
  test_cleanup ();
  return 0;
}