bool Item_cache_wrapper::get_date(MYSQL_TIME *ltime, ulonglong fuzzydate)
{
  Item *cached_value;
  DBUG_ENTER("Item_cache_wrapper::get_date");
  if (!expr_cache)
    DBUG_RETURN((null_value= orig_item->get_date(ltime, fuzzydate)));

  if ((cached_value= check_cache()))
    DBUG_RETURN((null_value= cached_value->get_date(ltime, fuzzydate)));

  cache();
  DBUG_RETURN((null_value= expr_value->get_date(ltime, fuzzydate)));
}