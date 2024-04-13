say (char const *format, ...)
{
  va_list args;
  va_start (args, format);
  vsay (format, args);
  va_end (args);
}