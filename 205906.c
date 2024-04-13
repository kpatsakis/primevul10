bool Item_cache_wrapper::val_bool()
{
  Item *cached_value;
  DBUG_ENTER("Item_cache_wrapper::val_bool");
  if (!expr_cache)
  {
    bool tmp= orig_item->val_bool();
    null_value= orig_item->null_value;
    DBUG_RETURN(tmp);
  }

  if ((cached_value= check_cache()))
  {
    bool tmp= cached_value->val_bool();
    null_value= cached_value->null_value;
    DBUG_RETURN(tmp);
  }
  cache();
  null_value= expr_value->null_value;
  DBUG_RETURN(expr_value->val_bool());
}