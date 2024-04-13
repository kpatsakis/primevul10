double Item_cache_wrapper::val_real()
{
  Item *cached_value;
  DBUG_ENTER("Item_cache_wrapper::val_real");
  if (!expr_cache)
  {
    double tmp= orig_item->val_real();
    null_value= orig_item->null_value;
    DBUG_RETURN(tmp);
  }

  if ((cached_value= check_cache()))
  {
    double tmp= cached_value->val_real();
    null_value= cached_value->null_value;
    DBUG_RETURN(tmp);
  }
  cache();
  null_value= expr_value->null_value;
  DBUG_RETURN(expr_value->val_real());
}