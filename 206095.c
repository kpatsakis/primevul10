add_interval(MYSQL_TIME *ltime, const Time_zone *time_zone,
             interval_type scale, INTERVAL interval)
{
  if (date_add_interval(ltime, scale, interval))
    return 0;

  uint not_used;
  return time_zone->TIME_to_gmt_sec(ltime, &not_used);
}