xsnprintf_func (char *str, int n, const char *fmt, ...)
{
  va_list a;
  int ret;
  va_start (a, fmt);
  ret = vsnprintf_func (str, n, fmt, a);
  va_end (a);
  if (ret < 0)
    {
      ret = n;
    }
  return ret;
}