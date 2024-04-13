append_datetime(String *buf, Time_zone *time_zone, my_time_t secs,
                const char *name, uint len)
{
  char dtime_buff[20*2+32];/* +32 to make my_snprintf_{8bit|ucs2} happy */
  buf->append(STRING_WITH_LEN(" "));
  buf->append(name, len);
  buf->append(STRING_WITH_LEN(" '"));
  /*
    Pass the buffer and the second param tells fills the buffer and
    returns the number of chars to copy.
  */
  MYSQL_TIME time;
  time_zone->gmt_sec_to_TIME(&time, secs);
  buf->append(dtime_buff, my_datetime_to_str(&time, dtime_buff, 0));
  buf->append(STRING_WITH_LEN("'"));
}