test_cleanup (void)
{
#if 1
  test_unlink (TEST_TARGET_FILE);
  test_unlink (TEST_SOURCE_FILE);
  test_unlink (TEST_DELTA_FILE);
  test_unlink (TEST_RECON_FILE);
  test_unlink (TEST_RECON2_FILE);
  test_unlink (TEST_COPY_FILE);
  test_unlink (TEST_NOPERM_FILE);
#endif
}