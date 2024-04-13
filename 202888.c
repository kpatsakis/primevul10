fatal (char const *format, ...)
{
  va_list args;
  fprintf (stderr, "%s: **** ", program_name);
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  putc ('\n', stderr);
  fflush (stderr);
  fatal_exit (0);
}