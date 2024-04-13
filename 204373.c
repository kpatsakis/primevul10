test_urlencoding (void)
{
  unsigned int errorCount = 0;

  errorCount += test_urlencoding_case (URL_START,
                                       URL_END,
                                       URL_DATA);
  errorCount += test_urlencoding_case (URL_NOVALUE1_START,
                                       URL_NOVALUE1_END,
                                       URL_NOVALUE1_DATA);
  errorCount += test_urlencoding_case (URL_NOVALUE2_START,
                                       URL_NOVALUE2_END,
                                       URL_NOVALUE2_DATA);
  if (0 != errorCount)
    fprintf (stderr,
             "Test failed in line %u with %u errors\n",
             (unsigned int) __LINE__,
             errorCount);
  return errorCount;
}