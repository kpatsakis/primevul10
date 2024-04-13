bool Item_field::get_date_result(MYSQL_TIME *ltime, ulonglong fuzzydate)
{
  if (result_field->is_null() || result_field->get_date(ltime,fuzzydate))
  {
    bzero((char*) ltime,sizeof(*ltime));
    return (null_value= 1);
  }
  return (null_value= 0);
}