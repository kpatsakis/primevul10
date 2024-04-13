xprintf (const char *fmt, ...)
{
  char buf[1000];
  va_list a;
  int size;
  va_start (a, fmt);
  size = vsnprintf_func (buf, 1000, fmt, a);
  va_end (a);
  if (size < 0)
    {
      size = sizeof(buf) - 1;
      buf[size] = 0;
    }
  if (xprintf_message_func != NULL) {
    xprintf_message_func(buf);
  } else {
    size_t ignore = fwrite(buf, 1, size, stderr);
    (void) ignore;
  }
}