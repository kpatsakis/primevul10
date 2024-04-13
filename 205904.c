bool  Item_cache_temporal::cache_value()
{
  if (!example)
    return false;

  value_cached= true;
 
  MYSQL_TIME ltime;
  if (example->get_date_result(&ltime, 0))
    value=0;
  else
    value= pack_time(&ltime);
  null_value= example->null_value;
  return true;
}