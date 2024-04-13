Item* Item::set_expr_cache(THD *thd)
{
  DBUG_ENTER("Item::set_expr_cache");
  Item_cache_wrapper *wrapper;
  if ((wrapper= new Item_cache_wrapper(this)) &&
      !wrapper->fix_fields(thd, (Item**)&wrapper))
  {
    if (wrapper->set_cache(thd))
      DBUG_RETURN(NULL);
    DBUG_RETURN(wrapper);
  }
  DBUG_RETURN(NULL);
}