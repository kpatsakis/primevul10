time_to_log_form(time_t t)
{
  struct tm stm;
  static char buffer[64];
  static const char *months[] = {"Jan", "Feb", "Mar", "Apr",
                                 "May", "Jun", "Jul", "Aug",
                                 "Sep", "Oct", "Nov", "Dec"};


  stm = *gmtime(&t);
  snprintf(buffer, sizeof(buffer),
          "%2d%s%02d %02d:%02d:%02d",
          stm.tm_mday, months[stm.tm_mon], stm.tm_year % 100,
          stm.tm_hour, stm.tm_min, stm.tm_sec);

  return buffer;
}