test_file_size (const char* file, xoff_t *size)
{
  struct stat sbuf;
  int ret;
  (*size) = 0;

  if (stat (file, & sbuf) < 0)
    {
      ret = get_errno ();
      XPR(NT "stat failed: %s: %s\n", file, strerror (ret));
      return ret;
    }

  if (! S_ISREG (sbuf.st_mode))
    {
      ret = XD3_INTERNAL;
      XPR(NT "not a regular file: %s: %s\n", file, strerror (ret));
      return ret;
    }

  (*size) = sbuf.st_size;
  return 0;
}