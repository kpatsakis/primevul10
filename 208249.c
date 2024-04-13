int test_setup (void)
{
  static int x = 0;
  x++;
  snprintf_func (TEST_TARGET_FILE, TESTFILESIZE, "/tmp/xdtest.target.%d", x);
  snprintf_func (TEST_SOURCE_FILE, TESTFILESIZE, "/tmp/xdtest.source.%d", x);
  snprintf_func (TEST_DELTA_FILE, TESTFILESIZE, "/tmp/xdtest.delta.%d", x);
  snprintf_func (TEST_RECON_FILE, TESTFILESIZE, "/tmp/xdtest.recon.%d", x);
  snprintf_func (TEST_RECON2_FILE, TESTFILESIZE, "/tmp/xdtest.recon2.%d", x);
  snprintf_func (TEST_COPY_FILE, TESTFILESIZE, "/tmp/xdtest.copy.%d", x);
  snprintf_func (TEST_NOPERM_FILE, TESTFILESIZE, "/tmp/xdtest.noperm.%d", x);
  test_cleanup();
  return 0;
}