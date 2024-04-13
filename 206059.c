bool Item_cache_temporal::get_date(MYSQL_TIME *ltime, ulonglong fuzzydate)
{
  ErrConvInteger str(value);

  if (!has_value())
  {
    bzero((char*) ltime,sizeof(*ltime));
    return 1;
  }

  unpack_time(value, ltime);
  ltime->time_type= mysql_type_to_time_type(field_type());
  if (ltime->time_type == MYSQL_TIMESTAMP_TIME)
  {
    ltime->hour+= (ltime->month*32+ltime->day)*24;
    ltime->month= ltime->day= 0;
  }
  return 0;
 
}