int spprintf(char **pbuf, size_t max_len, const char *format, ...)
{
  int cc;
  va_list ap;

  va_start(ap, format);
  cc = vspprintf(pbuf, max_len, format, ap);
  va_end(ap);
  return (cc);
}