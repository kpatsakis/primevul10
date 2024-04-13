bool Item_ref::get_date(MYSQL_TIME *ltime,ulonglong fuzzydate)
{
  return (null_value=(*ref)->get_date_result(ltime,fuzzydate));
}