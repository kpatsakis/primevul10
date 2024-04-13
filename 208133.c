test_copy_to (const char *from, const char *to)
{
  char buf[TESTBUFSIZE];
  int ret;

  snprintf_func (buf, TESTBUFSIZE, "cp -f %s %s", from, to);

  if ((ret = system (buf)) != 0)
    {
      return XD3_INTERNAL;
    }

  return 0;
}