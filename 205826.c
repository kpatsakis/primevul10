String *Item_cache_wrapper::val_str(String* str)
{
  Item *cached_value;
  DBUG_ENTER("Item_cache_wrapper::val_str");
  if (!expr_cache)
  {
    String *tmp= orig_item->val_str(str);
    null_value= orig_item->null_value;
    DBUG_RETURN(tmp);
  }

  if ((cached_value= check_cache()))
  {
    String *tmp= cached_value->val_str(str);
    null_value= cached_value->null_value;
    DBUG_RETURN(tmp);
  }
  cache();
  if ((null_value= expr_value->null_value))
    DBUG_RETURN(NULL);
  DBUG_RETURN(expr_value->val_str(str));
}