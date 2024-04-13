Item *Item_cache_wrapper::check_cache()
{
  DBUG_ENTER("Item_cache_wrapper::check_cache");
  if (expr_cache)
  {
    Expression_cache_tmptable::result res;
    Item *cached_value;
    init_on_demand();
    res= expr_cache->check_value(&cached_value);
    if (res == Expression_cache_tmptable::HIT)
      DBUG_RETURN(cached_value);
  }
  DBUG_RETURN(NULL);
}