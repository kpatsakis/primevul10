vsay (char const *format, va_list args)
{
  vfprintf (stdout, format, args);
  fflush (stdout);
}