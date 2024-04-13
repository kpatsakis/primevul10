bool Item_ref_null_helper::get_date(MYSQL_TIME *ltime, ulonglong fuzzydate)
{  
  return (owner->was_null|= null_value= (*ref)->get_date_result(ltime, fuzzydate));
}