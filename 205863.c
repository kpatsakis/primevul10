void Item_param::set_time(MYSQL_TIME *tm, timestamp_type time_type,
                          uint32 max_length_arg)
{ 
  DBUG_ENTER("Item_param::set_time");

  value.time= *tm;
  value.time.time_type= time_type;

  if (value.time.year > 9999 || value.time.month > 12 ||
      value.time.day > 31 ||
      (time_type != MYSQL_TIMESTAMP_TIME && value.time.hour > 23) ||
      value.time.minute > 59 || value.time.second > 59 ||
      value.time.second_part > TIME_MAX_SECOND_PART)
  {
    ErrConvTime str(&value.time);
    make_truncated_value_warning(current_thd, MYSQL_ERROR::WARN_LEVEL_WARN,
                                 &str, time_type, 0);
    set_zero_time(&value.time, MYSQL_TIMESTAMP_ERROR);
  }

  state= TIME_VALUE;
  maybe_null= 0;
  max_length= max_length_arg;
  decimals= tm->second_part > 0 ? TIME_SECOND_PART_DIGITS : 0;
  DBUG_VOID_RETURN;
}