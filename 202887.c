pfatal (char const *format, ...)
{
  int errnum = errno;
  va_list args;
  fprintf (stderr, "%s: **** ", program_name);
  va_start (args, format);
  vfprintf (stderr, format, args);
  va_end (args);
  fflush (stderr); /* perror bypasses stdio on some hosts.  */
  errno = errnum;
  perror (" ");
  fflush (stderr);
  fatal_exit (0);
}