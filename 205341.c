void die(const char *fmt, ...)
{
  char buff[DIE_BUFF_SIZE];
  va_list args;
  va_start(args, fmt);
  make_error_message(buff, sizeof(buff), fmt, args);
  really_die(buff);
}