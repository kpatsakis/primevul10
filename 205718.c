bool Item_cache_wrapper::set_cache(THD *thd)
{
  DBUG_ENTER("Item_cache_wrapper::set_cache");
  DBUG_ASSERT(expr_cache == 0);
  expr_cache= new Expression_cache_tmptable(thd, parameters, expr_value);
  DBUG_RETURN(expr_cache == NULL);
}