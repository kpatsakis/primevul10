test_unlink (char* file)
{
  int ret;
  if ((ret = unlink (file)) != 0 && errno != ENOENT)
    {
      XPR(NT "unlink %s failed: %s\n", file, strerror(ret));
    }
}