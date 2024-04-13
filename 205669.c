longlong Item_cache_wrapper::val_int()
{
  Item *cached_value;
  DBUG_ENTER("Item_cache_wrapper::val_int");
  if (!expr_cache)
  {
    longlong tmp= orig_item->val_int();
    null_value= orig_item->null_value;
    DBUG_RETURN(tmp);
  }

  if ((cached_value= check_cache()))
  {
    longlong tmp= cached_value->val_int();
    null_value= cached_value->null_value;
    DBUG_RETURN(tmp);
  }
  cache();
  null_value= expr_value->null_value;
  DBUG_RETURN(expr_value->val_int());
}