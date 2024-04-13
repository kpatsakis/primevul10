get_errno (void)
{
#ifndef _WIN32
  if (errno == 0)
    {
      XPR(NT "you found a bug: expected errno != 0\n");
      errno = XD3_INTERNAL;
    }
  return errno;
#else
  DWORD err_num = GetLastError();
  if (err_num == NO_ERROR)
    {
      err_num = XD3_INTERNAL;
    }
  return err_num;
#endif
}