bool Item_cache_wrapper::is_null()
{
  Item *cached_value;
  DBUG_ENTER("Item_cache_wrapper::is_null");
  if (!expr_cache)
  {
    bool tmp= orig_item->is_null();
    null_value= orig_item->null_value;
    DBUG_RETURN(tmp);
  }

  if ((cached_value= check_cache()))
  {
    bool tmp= cached_value->is_null();
    null_value= cached_value->null_value;
    DBUG_RETURN(tmp);
  }
  cache();
  DBUG_RETURN((null_value= expr_value->null_value));
}